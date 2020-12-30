import os
import requests
from bs4 import BeautifulSoup

path = "initfs/tools"
headers = { 'User-Agent': 'PostmanRuntime/7.26.8' }
packages = { 'ntfs-3g': 'https://debian.pkgs.org/sid/debian-main-amd64/ntfs-3g_2017.3.23AR.3-3_amd64.deb.html' }

def download(url):
    name = url.split('/')[-1].split('.html')[0]
    files = os.listdir(path)
    print('Checking for: '+name)

    if name not in files and 'dpkg' not in name:
        page = requests.request("GET", url, headers=headers)
        soup = BeautifulSoup(page.content, 'html.parser')
        download = soup.find_all('td')

        for link in download:
            if 'http' in link.text and 'deb' in link.text:
                os.system(f'wget {link.text} -P {path}')
                break
    else: print(name+' have been downloaded.\n')

def getLink(url):
    page = requests.request("GET", url, headers=headers)
    soup = BeautifulSoup(page.content, 'html.parser')
    a = soup.find(id="distro-88")
    td = a.find_all('td', {'class': 'w-50 pl-4'})

    download(td[0].find('a')['href'])
    #requires(td[0].find('a')['href'])
    
def requires(url):
    page = requests.request("GET", url, headers=headers)
    soup = BeautifulSoup(page.content, 'html.parser')
    tables = soup.find_all('tbody', {"class": "text-break"})
    a = tables[1].find_all('a')

    for link in a:
        getLink(link['href'])

#for key, value in packages.items():
#    download(value)
#    requires(value)

url = 'https://debian.pkgs.org/sid/debian-main-amd64/libk5crypto3_1.18.3-4_amd64.deb.html'
download(url)
requires(url)