/*
Szymon Rusinkiewicz
Princeton University

results2svg.cc
Create an SVG table from a file of icprobustness results
*/

#include "Color.h"
#include "colormap_CET_L19.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>
#include "unistd.h"
using namespace std;
using namespace trimesh;


// White-to-red colormap
Color colormap(float f)
{
#if 0
	Color good(1.0f, 1.0f, 1.0f);
	Color bad(0.7f, 0.1f, 0.1f);

	const float g = 0.5f;
	good = good.ungamma(g);
	bad = bad.ungamma(g);
	return mix(bad, good, f).gamma(g);
#else
	f = sqr(f);
	int n = 255 - clamp(int(f * 256), 0, 255);
	return Color(colormap_CET_L19[n]);
#endif
}


void usage(const char *myname)
{
	fprintf(stderr, "Usage: %s [-t][-l] in1.txt [in2.txt ...] out.svg\n", myname);
	exit(1);
}

int main(int argc, char *argv[])
{
	if (argc < 3)
		usage(argv[0]);

	bool top_header = false, left_header = false;
	int opt;
	while ((opt = getopt(argc, argv, "tl")) != EOF) {
		if (opt == 't')
			top_header = true;
		else if (opt == 'l')
			left_header = true;
	}

	vector<int> angles, translations;
	int nangles = 0, ntranslations = 0;
	vector< vector<float> > frac;

	// Read input files
	int nfiles = argc - 1 - optind;
	for (int file = 0; file < nfiles; file++) {
		FILE *f = fopen(argv[file+optind], "r");
		if (!f) {
			fprintf(stderr, "Couldn't open %s\n", argv[file+optind]);
			exit(1);
		}

		char buf[1024];
		int i = 0;
		fgets(buf, 1024, f);
		char *c = strtok(buf, " \n");
		while (c) {
			if (!file) {
				angles.push_back(atoi(c));
			} else {
				if (angles[i] != atoi(c)) {
					fprintf(stderr, "Angle mismatch.\n");
					exit(1);
				}
			}
			i++;
			c = strtok(NULL, " \n");
		}
		if (!file) {
			nangles = i;
		} else {
			if (i != nangles) {
				fprintf(stderr, "# Angles mismatch.\n");
				exit(1);
			}
		}

		int j = 0;
		fgets(buf, 1024, f);
		c = strtok(buf, " \n");
		while (c) {
			if (!file) {
				translations.push_back(atoi(c));
			} else {
				if (translations[j] != atoi(c)) {
					fprintf(stderr, "Translations mismatch.\n");
					exit(1);
				}
			}
			j++;
			c = strtok(NULL, " \n");
		}
		if (!file) {
			ntranslations = j;
		} else {
			if (j != ntranslations) {
				fprintf(stderr, "# Translations mismatch.\n");
				exit(1);
			}
		}


		if (!file)
			frac.resize(nangles);
		for (i = 0; i < nangles; i++) {
			if (!file)
				frac[i].resize(ntranslations);
			for (j = 0; j < ntranslations; j++) {
				float this_frac;
				fscanf(f, "%f", &this_frac);
				frac[i][j] += this_frac;
			}
		}
		fclose(f);
	}

	// Write output
	const char *font = "Helvetica";
	const int cell_size = 100;
	const int cell_font = 40;
	const int header_size = 56;
	const int header_font = 36;

	FILE *out = fopen(argv[argc-1], "w");
	int w = header_size + cell_size * ntranslations;
	int h = header_size + cell_size * nangles;
	fprintf(out, "<svg viewBox=\"0 0 %d %d\" xmlns=\"http://www.w3.org/2000/svg\">\n",
		w, h);

	if (top_header) {
		for (int j = 0; j < ntranslations; j++) {
			int x = header_size + j * cell_size + cell_size / 2;
			int y = header_size / 2;
			fprintf(out, "<text x=\"%d\" y=\"%d\"", x, y);
			fprintf(out, " font-family=\"%s\" font-size=\"%d\"",
				font, header_font);
			fprintf(out, " text-anchor=\"middle\" dominant-baseline=\"middle\">");
			fprintf(out, "%d%%</text>\n", translations[j]);
		}
	}

	for (int i = 0; i < nangles; i++) {
		int x = header_size / 2;
		int y = header_size + i * cell_size + cell_size / 2;

		if (left_header) {
			fprintf(out, "<text x=\"%d\" y=\"%d\"", x, y);
			fprintf(out, " font-family=\"%s\" font-size=\"%d\"",
				font, header_font);
			fprintf(out, " text-anchor=\"middle\" dominant-baseline=\"middle\">");
			fprintf(out, "%d&#176;</text>\n", angles[i]);
		}

		for (int j = 0; j < ntranslations; j++) {
			int x0 = header_size + j * cell_size;
			int y0 = header_size + i * cell_size;
			x = x0 + cell_size / 2;
			// y already set correctly
			float this_frac = frac[i][j] / nfiles;
			Color col = colormap(this_frac);
			fprintf(out, "<rect x=\"%d\" y=\"%d\"", x0, y0);
			fprintf(out, " width=\"%d\" height=\"%d\"",
				cell_size, cell_size);
			fprintf(out, " fill=\"#%02x%02x%02x\"/>\n",
				clamp(int(col[0] * 256.0f), 0, 255),
				clamp(int(col[1] * 256.0f), 0, 255),
				clamp(int(col[2] * 256.0f), 0, 255));
			fprintf(out, "<text x=\"%d\" y=\"%d\"", x, y);
			fprintf(out, " font-family=\"%s\" font-size=\"%d\"",
				font, cell_font);
			fprintf(out, " text-anchor=\"middle\" dominant-baseline=\"middle\">");
			fprintf(out, "%d</text>\n", (int)round(100.0f * this_frac));
		}
	}

#if 0
	int x1 = 0, x2 = header_size + cell_size * ntranslations;
	int y1 = header_size, y2 = y1;
	fprintf(out, "<line x1=\"%d\" y1=\"%d\" x2=\"%d\" y2=\"%d\"",
		x1, y1, x2, y2);
	fprintf(out, " stroke=\"#666\" stroke-width=\"2\"/>\n");

	x1 = header_size, x2 = x1;
	y1 = 0, y2 = header_size + cell_size * nangles;
	fprintf(out, "<line x1=\"%d\" y1=\"%d\" x2=\"%d\" y2=\"%d\"",
		x1, y1, x2, y2);
	fprintf(out, " stroke=\"#666\" stroke-width=\"2\"/>\n");
#endif
#if 1
	int x1 = header_size, x2 = header_size + cell_size * ntranslations;
	int y1 = header_size, y2 = y1;
	fprintf(out, "<line x1=\"%d\" y1=\"%d\" x2=\"%d\" y2=\"%d\"",
		x1, y1, x2, y2);
	fprintf(out, " stroke=\"#444\" stroke-width=\"2\"/>\n");

	y1 += cell_size * nangles;
	y2 = y1;
	fprintf(out, "<line x1=\"%d\" y1=\"%d\" x2=\"%d\" y2=\"%d\"",
		x1, y1, x2, y2);
	fprintf(out, " stroke=\"#444\" stroke-width=\"2\"/>\n");

	x1 = header_size, x2 = x1;
	y1 = header_size, y2 = header_size + cell_size * nangles;
	fprintf(out, "<line x1=\"%d\" y1=\"%d\" x2=\"%d\" y2=\"%d\"",
		x1, y1, x2, y2);
	fprintf(out, " stroke=\"#444\" stroke-width=\"2\"/>\n");

	x1 += cell_size * ntranslations;
	x2 = x1;
	fprintf(out, "<line x1=\"%d\" y1=\"%d\" x2=\"%d\" y2=\"%d\"",
		x1, y1, x2, y2);
	fprintf(out, " stroke=\"#444\" stroke-width=\"2\"/>\n");
#endif

	fprintf(out, "</svg>\n");
	fclose(out);
}

