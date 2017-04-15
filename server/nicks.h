/*
 * This file provides an interface for securing a random nick name for an AI. The 
 * names provided come from:
 * https://en.wikipedia.org/w/index.php?title=Wikipedia:List_of_Wikipedians_by_number_of_edits/9001–10000&oldid=767993622
 * This is the list of the top editors of Wikipedia in March 2017, number 9001-1000
 */

#ifndef NICKS_H
#define NICKS_H

#include <QString>

QString getRandomNick();

#endif // !NICKS_H
