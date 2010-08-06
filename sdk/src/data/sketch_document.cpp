#include "onyx/data/sketch_document.h"
#include "onyx/data/sketch_io.h"

namespace sketch
{

SketchDocument::SketchDocument(void)
{
}

SketchDocument::~SketchDocument(void)
{
    close();
}

bool SketchDocument::open(const QString & path)
{
    if (!close())
    {
        return false;
    }
    path_ = path;
    return true;
}

bool SketchDocument::close()
{
    clearPages();
    path_.clear();
    return true;
}

void SketchDocument::clearPages()
{
    activated_pages_.clear();
    pages_.clear();
}

SketchPagePtr SketchDocument::getPage(const PageKey & key,
                                      bool create,
                                      const QString & background_image)
{
    // get a page by key
    PagesIter idx = pages_.find(key);
    if (idx != pages_.end())
    {
        return idx.value();
    }

    if (create)
    {
        // if the page is empty, create a new one
        SketchPagePtr page(new SketchPage());
        page->setKey(key);

        QString bk_image_path = background_image;
        if (bk_image_path.isEmpty())
        {
            // if no additional back ground image path is set, set document path as default value
            bk_image_path = path_;
        }
        page->setBackgroundImage(bk_image_path);
        pages_[key] = page;
        return page;
    }
    return SketchPagePtr();
}

SketchPagePtr SketchDocument::insertPage(const PageKey & key, const QString & background_image)
{
    // query whether the page with this key existed
    SketchPagePtr page = getPage(key, false, background_image);
    if (page != 0)
    {
        // if the page exists, move the pages after this one forward
        int insert_page_number = key.toInt();
        Pages updated_pages;
        PagesIter idx = pages_.begin();
        for (; idx != pages_.end(); idx++)
        {
            SketchPagePtr page = idx.value();
            int page_number = page->key().toInt();
            if (page_number < insert_page_number)
            {
                updated_pages[page->key()] = page;
            }
            else
            {
                // move the page forward
                PageKey updated_page_key;
                updated_page_key.setNum(page_number + 1);
                page->setKey(updated_page_key);
                updated_pages[updated_page_key] = page;
                page->setKeyDirty(true);
            }
        }

        // update the pages map and insert the new page
        pages_ = updated_pages;
    }
    return getPage(key, true, background_image);
}

bool SketchDocument::removePage(const PageKey & key, SketchIOPtr io)
{
    // query whether the page with this key existed
    SketchPagePtr page = getPage(key, false, QString());
    if (page != 0)
    {
        // if the page exists, remove the data
        page->clearStrokes();
        page->setDataDirty(true);
        if (io != 0)
        {
            io->savePage(page);
        }

        // , move the pages after this one backward
        int remove_page_number = key.toInt();
        Pages updated_pages;
        PagesIter idx = pages_.begin();
        for (; idx != pages_.end(); idx++)
        {
            SketchPagePtr page = idx.value();
            int page_number = page->key().toInt();
            if (page_number < remove_page_number)
            {
                updated_pages[page->key()] = page;
            }
            else if (page_number > remove_page_number)
            {
                // move the page forward
                PageKey updated_page_key;
                updated_page_key.setNum(page_number - 1);
                page->setKey(updated_page_key);
                updated_pages[updated_page_key] = page;
                page->setKeyDirty(true);
            }
        }

        // update the pages map and insert the new page
        pages_ = updated_pages;
        return true;
    }
    return false;
}

bool SketchDocument::loadAllPages(SketchIOPtr io)
{
    return io->loadPages(pages_);
}

bool SketchDocument::hitTest(QMouseEvent *e, SketchPagePtr & page, SketchPosition & pos)
{
    PagesIter idx = activated_pages_.begin();
    for (; idx != activated_pages_.end(); idx++)
    {
        if (idx.value()->hitTest(e, pos))
        {
            page = idx.value();
            return true;
        }
    }
    return false;
}

void SketchDocument::updateSketchContext(const SketchContext & ctx)
{
    sketch_ctx_ = ctx;
}

void SketchDocument::updateEraseContext(const EraseContext & ctx)
{
    erase_ctx_ = ctx;
}

bool SketchDocument::isPageActive(const SketchPagePtr page)
{
    return (activated_pages_.find(page->key()) != activated_pages_.end());
}

bool SketchDocument::hasDirtyPages()
{
    PagesIter idx = pages_.begin();
    for (; idx != pages_.end(); idx++)
    {
        if (idx.value()->isDataDirty() || idx.value()->isKeyDirty() || idx.value()->isBackgroundDirty())
        {
            return true;
        }
    }
    return false;
}

bool SketchDocument::activatePage(const PageKey & page_key)
{
    SketchPagePtr page = getPage(page_key, true, QString());
    if (page == 0)
    {
        return false;
    }
    activated_pages_[page_key] = page;
    return true;
}

bool SketchDocument::deactivatePage(const PageKey & page_key)
{
    PagesIter idx = activated_pages_.find(page_key);
    if (idx == activated_pages_.end())
    {
        return false;
    }
    activated_pages_.erase(idx);
    return true;
}

void SketchDocument::deactivateAll()
{
    activated_pages_.clear();
}

PageKey SketchDocument::getLastPage()
{
    PageKey last_page;
    PagesIter idx = pages_.begin();
    PagesIter last_page_idx = idx;
    for (; idx != pages_.end(); idx++)
    {
        if (idx.value()->key().toInt() > last_page_idx.value()->key().toInt())
        {
            last_page_idx = idx;
        }
    }

    if (last_page_idx != pages_.end())
    {
        last_page = last_page_idx.value()->key();
    }
    return last_page;
}

PageKey SketchDocument::getFirstPage()
{
    PageKey first_page;
    PagesIter idx = pages_.begin();
    PagesIter first_page_idx = idx;
    for (; idx != pages_.end(); idx++)
    {
        if (idx.value()->key().toInt() < first_page_idx.value()->key().toInt())
        {
            first_page_idx = idx;
        }
    }

    if (first_page_idx != pages_.end())
    {
        first_page = first_page_idx.value()->key();
    }
    return first_page;
}

}
