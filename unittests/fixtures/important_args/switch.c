// FIRM_IMPORTANT_ARGS: 1

int f(int x);

int f(int x)
{
	switch (x) {
	case 0:
		return 1;
	case 1:
		return 0;
	default:
		return -1;
	}
}