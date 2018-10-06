#include <cstdio>
#include <cstring>

using std::fprintf;
using std::strncmp;

static void usage()
{
	fprintf(stdout,
		"Usage:\n\thuffman -e originalfile encodedfile"
		"\n\thuffman -d encodedfile decodedfile\n");
}

int main(int argc, char** argv)
{
	/* argument checking / usage */
	if (argc != 4)
	{
		fprintf(stderr, "E: %s takes 3 args. %d found.\n", argv[0], argc - 1);
		usage();
		return 1;
	}
	
	/* handle decode */
	if (!strncmp("-d", argv[1], 3))
	{

	}
	/* handle encode */
	else if (!strncmp("-e", argv[1], 3))
	{
		
	}
	else
	{
		usage();
		return 2;
	}

	return 0;
}
