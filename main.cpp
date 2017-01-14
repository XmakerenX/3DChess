#include <iostream>
#include <string>
#include <fontconfig/fontconfig.h>
#include "GameWin.h"

void printAllFonts()
{
    FcConfig* config = FcInitLoadConfigAndFonts();
    FcPattern* pat = FcPatternCreate();
    FcObjectSet* os = FcObjectSetBuild (FC_FAMILY, FC_STYLE, FC_LANG, FC_FILE, (char *) 0);
    FcFontSet* fs = FcFontList(config, pat, os);
    printf("Total matching fonts: %d\n", fs->nfont);
    for (int i=0; fs && i < fs->nfont; ++i) {
       FcPattern* font = fs->fonts[i];
       FcChar8 *file, *style, *family;
       if (FcPatternGetString(font, FC_FILE, 0, &file) == FcResultMatch &&
           FcPatternGetString(font, FC_FAMILY, 0, &family) == FcResultMatch &&
           FcPatternGetString(font, FC_STYLE, 0, &style) == FcResultMatch)
       {
          printf("Filename: %s (family %s, style %s)\n", file, family, style);
       }
    }
    if (fs) FcFontSetDestroy(fs);
}

void findFont(std::string name,std::string& path)
{
    FcConfig* config = FcInitLoadConfigAndFonts();
    FcResult result;
    // configure the search pattern,
    // assume "name" is a std::string with the desired font name in it
    FcPattern* pat = FcNameParse((const FcChar8*)(name.c_str()));
    FcConfigSubstitute(config, pat, FcMatchPattern);
    FcDefaultSubstitute(pat);

    // find the font
    FcPattern* font = FcFontMatch(config, pat, &result);
    if (font)
    {
       FcChar8* file = NULL;
       if (FcPatternGetString(font, FC_FILE, 0, &file) == FcResultMatch)
       {
          // save the file to another std::string
          path = (char*)file;
       }
       FcPatternDestroy(font);
    }

    FcPatternDestroy(pat);
}

int main(int argc, char* argv[])
{
    GameWin gamewin;
    std::string fontName = "NotoMono";
    std::string path;

    std::cout << "starting init window\n";

    gamewin.initWindow();
    std::cout << "starting init OpenGL\n";
    gamewin.initOpenGL(800,600);

    std::cout << "Beginning game\n";
    //printAllFonts();
    findFont(fontName, path);
    std::cout << "path: " << path << "\n";
    int retCode = gamewin.BeginGame();

    if (!gamewin.Shutdown())
        return 1;

    return retCode;
}

