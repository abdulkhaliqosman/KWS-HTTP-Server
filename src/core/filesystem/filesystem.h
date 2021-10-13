#include <cstdio>

namespace ak::core
{
    int readfile(const char *filename, char *filebuf, int filebufsize)
    {
        FILE *fd = fopen(filename, "r");

        // obtain file size:
        fseek(fd, 0, SEEK_END);
        const int filelen = ftell(fd);
        rewind(fd);

        fread(filebuf, 1, filelen, fd);
        fclose(fd);

        return filelen;
    }

} // namespace ak::core
