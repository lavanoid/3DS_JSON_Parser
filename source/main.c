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

const char* parse_json(char* value) {

    u8* file_buffer; // Cast?
    FILE *file = fopen(JSON_FILE,"rb"); // Open the file.
    fseek(file,0,SEEK_END); // Get the file size.
    off_t size = ftell(file);
    fseek(file,0,SEEK_SET);
    file_buffer=malloc(size);
    
    if(!file_buffer) { // Check if the file was copied into the buffer.
        printf("Could not buffer the file.\n");
        return "Error.";
    }
    
    off_t bytesRead = fread(file_buffer,1,size,file);
    fclose(file);
    
    if(size!=bytesRead) {
        printf("Could not read the file.\n"); // Check if the size is correct.
        return "Error.";
    }
    
    int i;
    int r;
    jsmn_parser p; // Start new JSON thingy.
    jsmntok_t t[128]; // We don't expect any more than 128 tokens.
    jsmn_init(&p); // Initialise json tokeniser.
    r = jsmn_parse(&p, file_buffer, size, t, sizeof(t)/sizeof(t[0]));
    
    if (r < 0) {
        return "Failed to parse json.";
    }
    
    if (r < 1 || t[0].type != JSMN_OBJECT) {
        return "Object expected.";
    }
    static char *tempString;
    for (i = 1; i < r; i++) { // Cycle through tokens until needed value is found.
        if (jsoneq(file_buffer, &t[i], value) == 0) {
            tempString = (char *)malloc(t[i+1].end-t[i+1].start+1);
            if(tempString != NULL) {
                sprintf(tempString,"%s", file_buffer + t[i+1].start);
                return (const char *)tempString;
            } else {
                return "malloc error!";
            }
        }
    }
    
}

int main() {
    gfxInitDefault();
    consoleInit(GFX_TOP,NULL);
    printf("Freq: %s\n",parse_json("frequency"));
    printf("Done\n");
    while (aptMainLoop()) {
        hidScanInput();
        u32 kDown = hidKeysDown();
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