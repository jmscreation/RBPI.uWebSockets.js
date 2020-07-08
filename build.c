#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

/* List of platform features */
#ifdef __arm__
#define IS_ARM
#endif

#ifdef __linux
#define OS "linux"
#endif

/* System, but with string replace */
int run(const char *cmd, ...) {
    char buf[512];
    va_list args;
    va_start(args, cmd);
    vsprintf(buf, cmd, args);
    va_end(args);
    printf("--> %s\n\n", buf);
    return system(buf);
}
/* Same as run above, but retrieves the stdout into given output buffer */
void exec(const char* cmd, char* output, ...){
    char buf[512], out[512] = "";
    va_list args;
    va_start(args, output);
    vsprintf(buf, cmd, args);
    va_end(args);
    printf("--> %s\n\n", buf);
    
    
    FILE *proc = popen(buf, "r");
    if(fgets(out, sizeof(out), proc) != NULL){
        out[strlen(out) - 1] = 0;
    } else {
        printf("Error executing command");
    }
    pclose(proc);
    strcpy(output, out);
}

/* List of Node.js versions */
struct node_version {
    char *name;
    char *abi;
} versions[] = {
    {"v10.17.0", "64"},
    {"v11.15.0", "67"},
    {"v12.13.0", "72"},
    {"v13.1.0", "79"},
    {"v14.0.0", "83"}
};
struct git_repo {
    char *org;
    char *repo;
    char *directory;
} gitrepos[] = {
    {"uNetworking", "uWebSockets.js", "uWebSockets.js/"}, //First
    {"uNetworking", "uWebSockets", "uWebSockets.js/uWebSockets/"}, //Second
    {"uNetworking", "uSockets", "uWebSockets.js/uWebSockets/uSockets/"} // Third
};

int failed = 0;
char uWebSocketsJSVersion[64];

/* Downloads headers, creates folders */
void prepare() {
    { // Get version information
        printf("\033[1;34m Gathering uWebSockets.js latest version information...\033[0m \n");
        char buf[128];
        exec("curl -s https://api.github.com/repos/uNetworking/uWebSockets.js/releases | python -c \"import sys,json; print json.load(sys.stdin)[0]['tag_name']\"", buf);
        strcpy(uWebSocketsJSVersion, buf+1); // remove 'v' from the version info
    }
    
    if(!run("mkdir targets")){
        /* For all versions */
        for (unsigned int i = 0; i < sizeof(versions) / sizeof(struct node_version); i++) {
            if(run("curl -OJ https://nodejs.org/dist/%s/node-%s-headers.tar.gz", versions[i].name, versions[i].name)){
                printf("\033[1;31m Failed to download node-js library headers! [node version %s]\033[0m  These are needed for compilation!", versions[i].abi);
                failed++;
            }
            if(run("tar xzf node-%s-headers.tar.gz -C targets", versions[i].name)){
                printf("\033[1;31m Failed to extract the node-js library headers! [node version %s]\033[0m  These are needed for compilation!", versions[i].abi);
                failed++;
            }
        }
    }
    
    if(!run("mkdir dist")){
        FILE *package = fopen("dist/package.json", "w");
        fprintf(package, "{\"bundleDependencies\": false,\"deprecated\": false,\"main\": \"uws.js\",\"name\": \"uWebSockets.js\",\"version\": \"%s\"}", uWebSocketsJSVersion);
        fclose(package);
    }
    
    printf("\033[1;34m Cloning uWebSockets.js Latest Version: %s\033[0m \n", uWebSocketsJSVersion);
    /* Download/Clone uWebSockets Git Repos*/
    for(unsigned int i = 0; i < sizeof(gitrepos) / sizeof(struct git_repo); i++){
        run("git clone \"https://github.com/%s/%s\" %s", gitrepos[i].org, gitrepos[i].repo, gitrepos[i].directory);
    }
}

/* Cleanup Unecessary Files After Building */
void cleanup(){
    printf("\033[1;34m Cleaning up object files...\033[0m \n");
    run("rm -f *.o");
    printf("\033[1;34m Cleaning up archives...\033[0m \n");
    run("rm -f *.gz");
}

/* Cleanup All Files Before Building - User Requests A Fresh Build */
void cleanupEverything(){
    printf("\033[1;34m Cleaning up NodeJS target libraries...\033[0m \n");
    run("rm -Rf ./targets");
    printf("\033[1;34m Cleaning up previous distrobution...\033[0m \n");
    run("rm -Rf ./dist");
    printf("\033[1;34m Removing all repositories...\033[0m \n");
    run("rm -Rf ./uWebSockets.js");
    cleanup();
}


/* Build for Unix systems */
void build(char *compiler, char *cpp_compiler, char *cpp_linker, char *os, char *arch) {
    
    printf("\033[1;34m Building shared libraries...\033[0m \n");
    char *c_shared = "-DLIBUS_USE_LIBUV -DLIBUS_USE_OPENSSL -flto -O3 -c -fPIC -I uWebSockets.js/uWebSockets/uSockets/src uWebSockets.js/uWebSockets/uSockets/src/*.c uWebSockets.js/uWebSockets/uSockets/src/eventing/*.c uWebSockets.js/uWebSockets/uSockets/src/crypto/*.c";
    char *cpp_shared = "-DUWS_WITH_PROXY -DLIBUS_USE_LIBUV -DLIBUS_USE_OPENSSL -flto -O3 -c -fPIC -std=c++17 -I uWebSockets.js/uWebSockets/uSockets/src -I uWebSockets.js/uWebSockets/src uWebSockets.js/src/addon.cpp";

    for (unsigned int i = 0; i < sizeof(versions) / sizeof(struct node_version); i++) {
        printf("\033[1;34m Building UWS Node Library \"%s\"...\033[0m \n", versions[i].abi);
        if(
              run("%s %s -I targets/node-%s/include/node", compiler, c_shared, versions[i].name)
            | run("%s %s -I targets/node-%s/include/node", cpp_compiler, cpp_shared, versions[i].name)
            | run("%s %s %s -o dist/uws_%s_%s_%s.node", cpp_compiler, "-flto -O3 *.o -std=c++17 -shared", cpp_linker, os, arch, versions[i].abi)
        ){ // On Failure
            printf("\033[1;31m Failed to compile UWS Node Library \"%s\" \033[0m\n\n", versions[i].abi);
            run("rm dist/uws_%s_%s_%s.node", os, arch, versions[i].abi); // delete library
            failed++;
        } else {
            printf("\033[0;32m UWS Node Library \"%s\" Compiled Successfully! \033[0m\n\n", versions[i].abi);
        }
    }
}

void copy_files() {
    printf("\033[1;34m Copying files...\033[0m");
    if(run("cp uWebSockets.js/src/uws.js dist/uws.js")){
        printf("\033[1;31m Failed to copy \"uws.js\"\033[0m\n\n");
        failed++;
    }
}

int main(int argc, const char** argv) {
    
#ifndef IS_ARM
    printf("This application is for the Raspberry Pi only");
    return 1;
#endif

    if(argc > 1 && !strcmp(argv[1], "-clean")){
        printf("\033[0;33m[Cleaning Everything]\033[0m\n");
        cleanupEverything();
    }

    /* Raspberry Pi Linux */
    printf("\033[0;33m[Preparing]\033[0m\n");
    prepare();
    printf("\n\033[0;33m[Building]\033[0m\n");

    build("gcc", "g++", "-static-libstdc++ -static-libgcc -s", OS, "arm");
    
    printf("\n\033[0;33m[Finishing]\033[0m\n");
    copy_files();
    
    printf("\n\033[0;33m[Cleanup]\033[0m\n");
    cleanup();
    
    if(failed){
        printf("\033[1;31m Build Finished With %d Errors! \033[0m \n There were errors when building the uWebSockets.js NodeJS libraries. Please check the process runtime log for further details.\nIf this keeps happening, use \"%s -clean\".\n", failed, argv[0]);
        return 1;
    } else {
        printf("\033[1;32m Build Successful!\033[0m \nYou can copy the \"dist\" folder into your NodeJS imported modules folder to use uWebSockets.js.\n");
    }
}

