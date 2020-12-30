import subprocess
olemas_olevad_pakid=set()
def r(pakk):
    t=subprocess.run(["apt-get", "download", pakk])
    if t.stderr or t.returncode:
        #print("could not load package: Status: {p.returncode}, stdout: {p.stdout}, stderr: {p.stderr}".format(p=t), "paki nimi:", pakk)
        return False
    olemas_olevad_pakid.add(pakk)
    t=subprocess.run(["apt-cache", "depends", "-i", pakk], stdout=subprocess.PIPE)
    if t.stderr or t.returncode:
        print(t.stdout)
        exit(t.stderr)
    #print(":", t.stdout)
    needed_pacages=t.stdout.split(b"Depends: ")[1:]
    #print(pakk, "needs packages:", needed_pacages)
    for choices in needed_pacages:
        one_choice_made=False
        for needed_package in choices.split(b"\n"):
            needed_package=needed_package.strip(b"\n ")
            if needed_package in olemas_olevad_pakid or r(needed_package):
                one_choice_made=True
                break
        if not one_choice_made:
            print("PROBLEM: could not download any of", choices)
            return False
    return True
#for pakk in packages_to_download:
#    print("pakk:",pakk)
#    r(pakk)
r(input("package name:"))