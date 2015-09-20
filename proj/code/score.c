#include <stdio.h>

#include "rtc.h"
#include "score.h"

table_t* hs = NULL;

const char* scores_filename = "/home/lcom/arkanix/highscores.aks";

void score_create()
{
	hs = (table_t*) malloc(sizeof(table_t));

	hs->cur_size = 0;
	hs->length = 4;
	hs->max_size = 10;
	hs->table = (score_t*) calloc(hs->max_size, sizeof(score_t));
}

void score_write()
{
	FILE* fp = fopen(scores_filename, "wb");

	if (fp == NULL)
	{
		fclose(fp);
		return;
	}

	fwrite(&hs->cur_size, sizeof(unsigned), 1, fp);
	fwrite(&hs->max_size, sizeof(unsigned), 1, fp);
	fwrite(&hs->length, sizeof(unsigned), 1, fp);

	int i;

	for (i = 0; i < hs->cur_size; i++)
	{
		fwrite(hs->table[i].name, hs->length * sizeof(char), 1, fp);
		fwrite(&hs->table[i].points, sizeof(unsigned), 1, fp);
		fwrite(&hs->table[i].date, sizeof(rtc_time_t), 1, fp);
	}

	fclose(fp);
}

void score_destroy()
{
	if (hs == NULL)
	{
		return;
	}

	free(hs->table);
	free(hs);

	hs = NULL;
}

void score_read()
{
	FILE* fp = fopen(scores_filename, "rb");

	if (fp == NULL)
	{
		return;
	}

	fread(&hs->cur_size, sizeof(unsigned), 1, fp);
	fread(&hs->max_size, sizeof(unsigned), 1, fp);

	if (!hs->max_size || !hs->cur_size)
	{
		return;
	}

	fread(&hs->length, sizeof(unsigned), 1, fp);

	if (!hs->length)
	{
		fclose(fp);
		return;
	}

	hs->table = (score_t*) calloc(hs->max_size, sizeof(score_t));

	int i;

	for (i = 0; i < hs->cur_size; i++)
	{
		score_t new_score;

		new_score.name = (char*) malloc(hs->length * sizeof(char));

		fread(new_score.name, hs->length * sizeof(char), 1, fp);
		fread(&new_score.points, sizeof(unsigned), 1, fp);
		fread(&new_score.date, sizeof(rtc_time_t), 1, fp);

		hs->table[i] = new_score;
	}

	fclose(fp);
}

static void score_sort()
{
	int i;
	int j;

	score_t s;

	for (i = 1; i < hs->cur_size; i++)
	{
		s = hs->table[i];
		j = i - 1;
		while ((j >= 0) && (s.points > hs->table[j].points))
		{
			hs->table[j + 1] = hs->table[j];
			j--;
		}
		hs->table[j + 1] = s;
	}
}

table_t* score_get()
{
	return hs;
}

__inline unsigned score_size()
{
	return hs->cur_size;
}

void score_add(char* player_name, int player_score)
{
	score_t new_score;

	new_score.name = player_name;
	new_score.date = rtc_current_date();
	new_score.points = player_score;

	if (hs->cur_size < hs->max_size)
	{
		hs->table[hs->cur_size] = new_score;
		hs->cur_size++;
		if (hs->cur_size > 1)
		{
			score_sort(hs);
		}
	}
	else if (hs->cur_size == hs->max_size)
	{
		score_sort();
		if (hs->table[hs->cur_size - 1].points < player_score)
		{
			hs->table[hs->cur_size - 1] = new_score;
			score_sort(hs);
		}
	}
}
