
#include <wave.h>
#include "to_pcm.h"
int main() {
    convertToPCM("file_example_MP3_1MG.mp3", "test.pcm");
    //to_pcm("file_example_MP3_1MG.mp3", "test.pcm");
    return 0;
}