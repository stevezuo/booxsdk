
#ifndef PAGINATOR_H_
#define PAGINATOR_H_

class Paginator
{
public:
    Paginator();
    ~Paginator();

public:
    void reset(int first, int items_per_page, int total);

    bool prev();
    bool next();
    bool jump(int new_page);

    bool isPrevEnable();
    bool isNextEnable();

    void resize(int new_items_per_page);

    int currentPage();
    int pages();

    int first_visible() { return first_visible_; }
    int last_visible();

    int offsetInCurrentPage();

    int size() { return size_; }
    int items_per_page() { return items_per_page_; }

private:
    int first_visible_;
    int items_per_page_;
    int size_;

};

#endif
