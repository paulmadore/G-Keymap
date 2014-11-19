#include <stdio.h>
#include <errno.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <cstdlib>

extern "C" {
#ifdef HAVE_GETOPT_H
#  define HAVE_GETOPT_LONG
#  include <getopt.h>
#endif
#ifndef X_NOT_STDC_ENV
#       include <stdlib.h>
#else
	        extern char *getenv();
#endif

#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <semaphore.h>
}

#include <string>
#include <iostream>
#include "mdloader.h"
#include "lineak/configdirectives.h"
#include "lineak/lineak_core_functions.h"

using namespace std;
//using namespace lineak_core_functions;

//extern "C" bool verbose;
bool verbose = false;
bool is_running(string process);

int main(int argc, char *argv[]) {

  map < string, ConfigDirectives* >* defs = NULL;
  map<int, ConfigDirectives*> heur;

  string home = getenv("HOME");
  string conffile = home + LINEAKDIR;
  conffile += "media-detect.conf";
  string arg = argv[1];
  
  MDLoader ldr(conffile);
  defs = ldr.loadDef();
  ConfigDirectives *dir = NULL;

  map < string, ConfigDirectives* >::iterator it = defs->begin();
  for (;it != defs->end(); it++) {
    dir = it->second;
    string name = dir->getValue(PROGRAM);
    cout << "Checking to see if: " << name << " is running." << endl;
    if (is_running(name)) {
	    int value = atoi((dir->getValue(PRIORITY)).c_str());
	    cout << "Found: " << name << " running. With priority: " << value << endl;
	    heur[value+1] = dir;
    }
  }
/*
  map <string,int>::iterator hit = heur.begin();
  for (; hit != heur.end(); hit++ ) {
     cout << hit->first << " = " << (hit->second)->getValue(NAME) << endl;
  }
*/
  dir = NULL;
  if (heur.size() == 1) {
	  dir = (heur.begin())->second;
  }
  else {
	  map<int, ConfigDirectives*>::iterator ita = heur.begin();
	  int pria = 0;
	  int prib = 0;
	  string name = "";

          for (; ita != heur.end(); ita++) {
	     // If there isn't an object pointed to yet, point to the first one.
             if (dir == NULL) {
                dir = ita->second;
	        pria = ita->first;
	     }
             else {
		prib = ita->first;
                if ( prib < pria ) {
		   pria = prib;
		   dir = ita->second;
		}
	     }
	  }
  }
/* 
  if ( arg != "" ) {
      string type = dir->getValue(TYPE);
      if (type == "dcop" || type == "DCOP" )
          dispatch_dcop(obj, command, dir);
      if (type == "program" || type == "PROGRAM" )
          dispatch_program(obj, command, dir);
      if (type == "macro" || type == "MACRO" )
          dispatch_macro(obj, command, dir);
  }
*/
  return 0;
}
/*
dispatch_dcop(LObject *obj, LCommand &command, ConfigDirectives *dir) {
}
dispatch_program(LObject *obj, LCommand &command, ConfigDirectives *dir) {
   const vector<string>& args = command.getArgs();
   string arg = args[1];
   string comm = dir->getValue(arg);
   string dname = dir->getValue(NAME);

   //obj->getType() == SYM
   if (global_enable) {
       if (!fork()) {
          comm += " &";
          // child process that tries to run the command, and then exits
          // all specials done, let's go for it... ;)
          if (verbose) cout << "... running " << comm << endl;
          if (verbose) cout << "... displaying " << dname << endl;
          system(comm.c_str());
          exit(true);
       }
       // NOTE: no, we don't wait() for the child. we just ignore the SIGCLD signal
       // Rotate the key state if it's a toggleable key
       if (obj->isUsedAsToggle())
          obj->toggleState();
       // Use the OSD to show the output
       display->show(dname);
   }
}
dispatch_macro(LObject *obj, LCommand &command, ConfigDirectives *dir) {
}
*/
bool is_running(string process) {
    uid_t myuid;
    pid_t mypid, pid;
    DIR *dir;
    struct dirent *d;
    struct stat s;
    string str, path;

    mypid = getpid();
    myuid = getuid();

   if ((dir = opendir("/proc")) == NULL)
   {
       cout << "Cannot open /proc" << endl;
       return -1;
   }

   while ((d = readdir(dir)) != NULL)
   {
       // See if this is a process
       if ((pid = atoi(d->d_name)) == 0)
            continue;
       //if (verbose) cout << "pid = " << pid << " mypid = " << mypid << endl;
       // Ignore the pid of the current running process
       if (pid == mypid) {
          //if (verbose) cout << "oops it's me, moving on." << endl;
          continue;
       }

       path = "/proc/" + string(d->d_name) + "/stat";

       // Only processes with the same UID as the current running process
       if (stat(path.c_str(), &s) != 0 && s.st_uid != myuid)
           continue;

       // Open the status file and verify the program name
       ifstream in(path.c_str());
       if (in.is_open()) {
          in >> str;
          in >> str;
          in.close();
	  //cout << "str:" << str << endl;
	  //cout << "(" << process << ")" << endl;
          //if (str.find("(" + process+")") != string::npos && (mypid != pid)) {
          if (str.find(process) != string::npos && (mypid != pid)) {
              if (verbose) cout << "*** " << process << " is running (pid " << pid << ")" << endl;
              if (verbose) cout << "*** mypid: " << mypid << endl;
              if (verbose) cout << str << endl;
               /* for (int i=0; i != 50; i++) {
                       sleep (5);
               } */
              return true;
          }
       }
   }
  return false;
}
