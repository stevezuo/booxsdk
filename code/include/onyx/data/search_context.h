#ifndef ONYX_SEARCH_CONTEXT_H_
#define ONYX_SEARCH_CONTEXT_H_

#include <QString>

struct SearchContext
{
    QString pattern;
    bool    forward;
    bool    match_case;
    bool    match_whole_word;

    SearchContext() : forward(false),
                      match_case(false),
                      match_whole_word(false) {}
};

#endif  // ONYX_SEARCH_CONTEXT_H_
