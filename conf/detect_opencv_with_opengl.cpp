#include <opencv2/cvconfig.h>

int main()
{
    #ifdef HAVE_OPENGL
        return 0;
    #else
        return 1;
    #endif
}
