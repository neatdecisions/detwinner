[![Stand With Ukraine](https://raw.githubusercontent.com/vshymanskyy/StandWithUkraine/main/badges/StandWithUkraine.svg)](https://stand-with-ukraine.pp.ua)

# Detwinner

Detwinner is a tool for the Linux desktop which allows searching and removing duplicate files and similar images.

## Main functionalities

As description suggests, Detwinner can search for duplicates in two modes:

1. Exact duplicates.
2. Similar images.

The mode can be selected using the toolbar from the main window of Detwinner. Each mode can be configured by clicking on a little settings icon next to it. These settings include:

* restrictions on the file size;
* including/excluding certain file attributes in the search;
* regular expressions to match file paths.

### Exact duplicates

In this mode Detwinner first arranges files according to their size, and then distributes them in groups by applying [Murmurhash](https://en.wikipedia.org/wiki/MurmurHash) on their content.

### Similar images

This mode introduces a couple of new settings:

* similarity level - shows to which extent two images should be similar in order to include them in the results (a value from 1 to 100);
* a setting which indicates whether the images should be considered as-is, without rotating them to find the best matching position.

Briefly the algorithm can be described as follows:

1. Split each image in 4 sections and compute 4 histograms (Y,U,V and intensity) for each of them.
2. Apply hierarchical clustering algorithm to the images using Hassanat distance between their respective histograms as a distance function.

### Processing results

The results of the search are presented in a window where duplicate files are organized in groups. The files can be previewed in the bottom pane. One of the previews is related to the file with a lock indicator nearby, another - to the currently selected files. The file locked for a preview can be changed by clicking on the lock icon.

Files to delete can be selected manually or using the smart select button in the toolbar (will apply the selection for all groups) or the selection menu which can be invoked by right-clicking on the duplicate group.

The selected files can be deleted permanently, moved to trash (not available in flatpak installation) or moved to a backup folder. If the last option is chosen, the full folder structure of the original files will be recreated in the selected folder.

## Compiling

A recent C++ compiler is required (at least C++14), together with [gtkmm](https://www.gtkmm.org/) of at least 3.22 version. Build process is handled by [meson](http://mesonbuild.com/).

`./configure.sh` script will create two folder (`Debug` and `Release`) with corresponding configurations. To trigger the build, run `ninja` in one of the folders. Running `ninja test` will obviously execute the unit tests as well.

## Acknowledgements

The nice frog images used in the unit tests are part of [GraphicsMagick](http://www.graphicsmagick.org/).
