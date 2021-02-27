#define _GNU_SOUCE
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/fsuid.h>
#include <limits.h>
#include <pwd.h>
#include <grp.h>


#define SG_SIZE  (NGROUPS_MAX +1)

int main(int argc, char** argv)
{
    uid_t ruid, euid, suid, fsuid;
    gid_t rgid, egid, sgid, fsgid;

    gid_t suppGroups[SG_SIZE];

    int numGroups, j;
    char *p;

    if (getresuid(&ruid, &euid, &suid) == -1)
        return -1;
    
    if (getresgid(&rgid, &egid, &sgid) == -1)
        return -2;

    struct passwd * pwd;
    pwd = getpwuid(ruid);
    if (pwd != NULL)
        printf("name is %s, ruid is %ld\n", pwd->pw_name, ruid);

    pwd = getpwuid(euid);
    if (pwd != NULL)
        printf("name is %s, euid is %ld\n", pwd->pw_name, euid);

    pwd = getpwuid(suid);
    if (pwd != NULL)
        printf("name is %s, suid is %ld\n", pwd->pw_name, suid);

    
    pwd = getpwuid(rgid);
    if (pwd != NULL)
        printf("name is %s, rgid is %ld\n", pwd->pw_name, rgid);

    pwd = getpwuid(egid);
    if (pwd != NULL)
        printf("name is %s, egid is %ld\n", pwd->pw_name, egid);

    pwd = getpwuid(sgid);
    if (pwd != NULL)
        printf("name is %s, sgid is %ld\n", pwd->pw_name, sgid);

    fsuid = setfsuid(0);
    fsgid = setfsgid(0);
    pwd = getpwuid(ruid);
    if (pwd != NULL)
        printf("name is %s, fsuid is %ld\n", pwd->pw_name, fsuid);
    pwd = getpwuid(fsgid);
    if (pwd != NULL)
        printf("name is %s, fsgid is %ld\n", pwd->pw_name, fsgid);

    
    numGroups = getgroups(SG_SIZE, suppGroups);
    if (numGroups == -1)
        return -3;

    printf("Supplementary groups:(%d): ", numGroups);
    for (j=0; j < numGroups; j++) {
        struct group *grp;
        grp = getgrgid(suppGroups[j]);
        printf("name is %s, suppGroups[j] is %ld\n", grp->gr_name, suppGroups[j]);        
        
    }
    
}