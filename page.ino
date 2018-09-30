#include "constants.h"
#include "common.h"

int page = -1;
int MAX_PAGE = 7;

void page_up()
{
	if (page == -1)
	{
		page = 0;
	}
	else 
	{
		page--;
		if (page < 0)
		{
			page = MAX_PAGE;
		}
	}

	indicatePage();
}

void page_down()
{
	if (page == -1)
	{
		page = 0;
	}
	else 
	{
		page++;
		if (page > MAX_PAGE)
		{
			page = 0;
		}
	}

	indicatePage();	
}

void indicatePage()
{
	for (int i = 0; i < 3; i++)
	{
		if (page % 3 != i)
		{
			set_led( i, 0 );
		}
		else
		{
			set_led( i, page < 3 ? 1 : 2 );
		}
	}	

	displayPage1();
}


