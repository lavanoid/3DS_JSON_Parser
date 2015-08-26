#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <inttypes.h>
#include <3ds.h>
#include "jsmn.h"
#define BUF_SIZE 1024

char JSON_FILE[] = "jsbr-ford-mustang.json";

static int jsoneq(const char *json, jsmntok_t *tok, const char *s) {
    if (tok->type == JSMN_STRING && (int) strlen(s) == tok->end - tok->start &&
        strncmp(json + tok->start, s, tok->end - tok->start) == 0) {
        return 0;
    }
    return -1;
}

const char * parse_json(char* value) {
    u8* file_buffer;
    FILE *file = fopen(JSON_FILE,"rb"); if (file == NULL) printf("Error.\n");
    fseek(file,0,SEEK_END); off_t size = ftell(file); fseek(file,0,SEEK_SET); file_buffer=malloc(size);
    if(!file_buffer) printf("Error.\n");
    off_t bytesRead = fread(file_buffer,1,size,file); fclose(file);
    if(size!=bytesRead) printf("Error.\n");
    int i; int r;
    jsmn_parser p; jsmntok_t t[128]; jsmn_init(&p);
    r = jsmn_parse(&p, file_buffer, size, t, sizeof(t)/sizeof(t[0]));
    
    if (r < 0) {
        printf("Failed to parse JSON: %d\n", r);
        return 1;
    }
    if (r < 1 || t[0].type != JSMN_OBJECT) {
        printf("Object expected\n");
        return 1;
    }
    for (i = 1; i < r; i++) {
        if (jsoneq(file_buffer, &t[i], value) == 0) {
            return printf("- Value: %.*s\n", t[i+1].end-t[i+1].start, file_buffer + t[i+1].start);
            i++;
        }
    }
}

int main() {
    gfxInitDefault();
    consoleInit(GFX_TOP,NULL);
    printf("P1\n");
    printf("%s",parse_json("description"));
    printf("P2\n");
    printf("%s",parse_json("synchronization_spacing_us"));
    while (aptMainLoop()) {
        hidScanInput(); u32 kDown = hidKeysDown();
        if(kDown & KEY_START) {
            consoleClear();
            break;
        }
        gfxFlushBuffers();
        gfxSwapBuffers();
    }
    gfxExit();
    return 0;
}