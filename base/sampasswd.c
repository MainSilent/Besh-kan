#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include "ntreg.h"
#include "sam.h"


const char sampasswd_version[] = "sampasswd version 0.2 140201, (c) Petter N Hagen";


/* Global verbosity flag */
int gverbose = 0;

/* Array of loaded hives */
#define MAX_HIVES 10
struct hive *hive[MAX_HIVES+1];
int no_hives = 0;

int H_SAM = -1;



int do_reset(char *user, int inrid, int verb)
{
  int rid = 0;
  int ret;
  char *resolvedname = NULL;
  char s[200];

  if ((H_SAM < 0) || (!user && !inrid) ) return(1);

  if (inrid) {
    rid = inrid;
  } else {
    if (*user == '0' && *(user+1) == 'x') sscanf(user,"%i",&rid);
  }

  if (!rid) { /* Look up username */
    /* Extract the unnamed value out of the username-key, value is RID  */
    snprintf(s,180,"\\SAM\\Domains\\Account\\Users\\Names\\%s\\@",user);
    rid = get_dword(hive[H_SAM],0,s, TPF_VK_EXACT|TPF_VK_SHORT);
    if (rid == -1) {
      printf("ERROR: User <%s> not found\n",user);
      return(1);
    }
  }

  /* At this point we have a RID, so get the real username it maps to just to show it */
  resolvedname = sam_get_username(hive[H_SAM], rid);

  if (!resolvedname) return(1);  /* RID lookup failed, no such user */

  if (gverbose) printf("do_reset: Username: %s, RID = %d (0x%0x)\n",resolvedname,rid,rid);
  
  ret = sam_reset_pw(hive[H_SAM], rid);

  if (!ret && verb) printf("Password reset for user %s, RID = %d [0x%0x]\n",resolvedname,rid,rid);
 
  FREE(resolvedname);
  return(ret);

}



void usage(void)
{
  printf(" [-r|-l] [-H] -u <user> <samhive>\n"
	 "Reset password or list users in SAM database\n"
         "Mode:\n"
	 "   -r = reset users password\n"
         "   -l = list users in sam\n"
         "Parameters:\n"
         "   <user> can be given as a username or a RID in hex with 0x in front\n"
         "   Example:\n"
         "   -r -u theboss -> resets password of user named 'theboss' if found\n"
         "   -r -u 0x3ea -> resets password for user with RID 0x3ea (hex)\n"
	 "   -r -a -> Reset password of all users in administrators group (0x220)\n"
	 "   -r -f -> Reset password of admin user with lowest RID\n"
	 "            not counting built-in admin (0x1f4) unless it is the only admin\n"
         "   Usernames with international characters usually fails to be found,\n"
         "   please use RID number instead\n"
	 "   If success, there will be no output, and exit code is 0\n"
	 "Options:\n"
	 "   -H : For list: Human readable listing (default is parsable table)\n"
	 "   -H : For reset: Will output confirmation message if success\n"
	 "   -N : No allocate mode, only allow edit of existing values with same size\n"
	 "   -E : No expand mode, do not expand hive file (safe mode)\n"
	 "   -t : Debug trace of allocated blocks\n"
	 "   -v : Some more verbose messages/debug\n"
	 );
}


int main(int argc, char **argv)
{
   
  extern int optind;
  extern char* optarg;

  int what = 0;
  int reset = 0;
  int list = 0;
  int mode = 0;
  int human = 0;
  int adm = 0;
  int all = 0;
  int first = 0;
  int ret, wret, il;
  char *hivename;
  char c;
  char *usr = NULL;

  char *options = "rlHu:vNEthaf";
  
  while((c=getopt(argc,argv,options)) > 0) {
    switch(c) {
    case 'r': reset = 1; break;
    case 'l': list  = 2; break;
    case 'u': usr = optarg; break;
    case 'a': all = 1; break;
    case 'f': first = 1; break;
    case 'H': human = 1; break;
    case 'v': mode |= HMODE_VERBOSE; gverbose = 1; break;
    case 'N': mode |= HMODE_NOALLOC; break;
    case 'E': mode |= HMODE_NOEXPAND; break;
    case 't': mode |= HMODE_TRACE; break;
    case 'h': printf("%s\n%s ",sampasswd_version,argv[0]); usage(); exit(0); break;
    default: printf("No arguments"); break;
    }
  }

  if (!reset && !list && !what) {
    fprintf(stderr,"%s: ERROR: Mode -r or -l must be specified. -h for help\n",argv[0]);
    exit(1);
  }

#if 0   /* Should both be allowed at same time?? */
  if (list && reset) {
    fprintf(stderr,"%s: ERROR: Mode -r and -l impossible at the same time. -h for help\n",argv[0]);
    exit(1);
  }
#endif

  if (reset && !all && !first && (!usr || !*usr)) {
    fprintf(stderr,"%s: ERROR: Need a user for reset, -u must be specified.\n",argv[0]);
    exit(1);
  }


  /* Load hives. Only first SAM hive will be used however */

  hivename = argv[optind+no_hives];
  if (!hivename || !*hivename) {
    fprintf(stderr,"%s: ERROR: You must specify a SAM registry hive filename.\n",argv[0]);
    exit(1);
  }
  do {
    if (!(hive[no_hives] = openHive(hivename,
				    HMODE_RW|mode))) {
      fprintf(stderr,"%s: ERROR: Unable to open/read registry hive, cannot continue\n",argv[0]);
      exit(1);
    }
    switch(hive[no_hives]->type) {
    case HTYPE_SAM:      H_SAM = no_hives; break;
      // case HTYPE_SOFTWARE: H_SOF = no_hives; break;
      // case HTYPE_SYSTEM:   H_SYS = no_hives; break;
      // case HTYPE_SECURITY: H_SEC = no_hives; break;
    }
    no_hives++;
    hivename = argv[optind+no_hives];
  } while (hivename && *hivename && no_hives < MAX_HIVES);

  if (H_SAM == -1) {
    fprintf(stderr,"%s: WARNING: Hive file does not look like SAM, but continuing anyway in case detection was wrong\n"
	    "%s: WARNING: If it really is not a SAM file you will get strange errors or bad results\n",argv[0],argv[0]);
    H_SAM = 0;
  }


  /* Do logic */

  if (list) {
    adm = sam_list_users(hive[H_SAM], human);
    if (gverbose) printf("   sam_list_users found admin to be 0x%x\n",adm);
  }

  if (reset) {
    if (all) {
      ret = sam_reset_all_pw(hive[H_SAM], human);
      if (ret) {
	fprintf(stderr,"%s: ERROR: Failed reset password of ALL users\n",argv[0]); 
      }
    } else if (first) {
      adm = sam_list_users(hive[H_SAM], 2);
      if (!adm) {
	fprintf(stderr,"%s: ERROR: Unable to reset, no admin users found\n",argv[0]);
      } else {
	//	printf("Resetting password of user with RID %x\n",adm);
	ret = do_reset(usr, adm, human);
      }
    } else {
      ret = do_reset(usr, 0, human);
      if (ret) {
	fprintf(stderr,"%s: ERROR: Failed reset password for %s\n",argv[0],usr); 
      }
    }
  }

  /* write registry hive (if needed) */

  wret = 0;
  for (il = 0; il < no_hives; il++) {
    //    wret |= writeHive(hive[il]);
    if (hive[il]->state & HMODE_DIDEXPAND)
      fprintf(stderr," WARNING: Registry file %s was expanded! Experimental! Use at own risk!\n",hive[il]->filename);  
    while (no_hives > 0)
      closeHive(hive[--no_hives]);
  }

  return(ret | wret);
}

