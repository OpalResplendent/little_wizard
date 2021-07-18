
#define LWD_MAX_FILENAME_LENGTH 32

//********************************************************************************/
// File Format Description:
// Header
// filenames
// 
//********************************************************************************/

union littlewizarddataheader {
    struct {
        char descriptor[4]; // should be LWD_, with _ being version
        uint32 filesfound;
        uint32 imagedo;
        uint32 spritedo;
        uint32 optimizationsdo;
        uint32 filenamesdo;
        uint8 padding[8];
    };
    uint8 fullsize[32];
};
