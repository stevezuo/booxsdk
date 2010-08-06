// Author: John

#include "onyx/cms/content_manager.h"
/*
namespace
{

TEST(CreateDatabase)
{
    // Make sure the content manager is able to create all
    // necessary tables.
    cms::ContentManager cm("content.db");
    cm.DropAllTables();
}

TEST(AddAndGet)
{
    cms::ContentManager cm("content.db");
    cms::ContentNode src;
    src.mutable_name() = "zeng";
    src.mutable_location() = "www.zeng.com";
    src.mutable_authors() = "zeng...";
    src.mutable_description() = "zeng zeng zeng";
    src.mutable_title() = "still zeng...";
    cm.CreateContentInfo(src);

    cms::ContentNode target;
    target.mutable_name() = src.name();
    target.mutable_location() = src.location();
    cm.GetContentInfo(target);
    assert(src == target);

    target.mutable_name().clear();
    cm.GetContentInfo(target);
    assert(! (target == src));

    src.mutable_name().clear();
    cm.UpdateContentInfo(src);
    cm.GetContentInfo(target);
    assert(src == target);

    cm.RemoveContent(src);
    assert(cm.GetContentInfo(target) == false);

    cm.DropAllTables();
}

TEST(AddCategory)
{
    using namespace cms;
    ContentManager cm("content.db");
    Category book_category;
    cm.CreateNewCategory(cm.mutable_local_root_category(),
        book_category);
    cms_ids children =
        cm.local_root_category().children_categories();

    cms_ids_iter iter = std::find(children.begin(), children.end(),
                                        book_category.id());
    assert (iter != children.end());
    cm.DropAllTables();
}

TEST(RemoveCategory)
{
    using namespace cms;
    ContentManager cm("content.db");
    Category book_category, news_category;
    cm.CreateNewCategory(cm.mutable_local_root_category(),
        book_category);

    cm.CreateNewCategory(cm.mutable_local_root_category(),
        news_category);

    cm.RemoveChildCategory(cm.mutable_local_root_category(),
        book_category);

    cms_ids children = cm.local_root_category().children_categories();

    cms_ids_iter iter = std::find(children.begin(), children.end(),
                                        book_category.id());
    assert (iter == children.end());

    iter = std::find(children.begin(), children.end(),
                    news_category.id());
    assert (iter != children.end());

    cm.DropAllTables();
}

TEST(AddChildContent)
{
    using namespace cms;
    ContentManager cm("content.db");
    Category book_category;
    cm.CreateNewCategory(cm.mutable_local_root_category(),
        book_category);

    ContentNode content;
    content.mutable_name() = "book";
    content.mutable_location() = "www.book.com";
    cm.CreateContentInfo(content);

    cm.AddChildContent(book_category, content);
    cms_ids children = book_category.children_categories();

    cms_ids_iter iter = std::find(children.begin(), children.end(),
        content.id());
    assert(iter != children.end());
    cm.DropAllTables();
}

TEST(RemoveChildContent)
{
    using namespace cms;
    ContentManager cm("content.db");
    Category book_category;
    cm.CreateNewCategory(cm.mutable_local_root_category(),
        book_category);

    ContentNode content;
    content.mutable_name() = "book";
    content.mutable_location() = "www.book.com";
    cm.CreateContentInfo(content);

    cm.AddChildContent(book_category, content);
    cms_ids children = book_category.children_categories();

    cms_ids_iter iter = std::find(children.begin(), children.end(),
        content.id());
    assert(iter != children.end());

    cm.RemoveChildContent(book_category, content);
    children = book_category.children_categories();

    iter = std::find(children.begin(), children.end(),
        content.id());
    assert(iter == children.end());
    cm.DropAllTables();
}

}   // anonymous name
*/


// I'm not able to run the above sample code in Windows, so
// I use the following code to do unit test.
int main(int argc, char *argv[])
{
    try
    {
        // test 1
        {

        }

        // test 2 content node
        {
            int i = 0;
            std::string name = "content.db";
            base::string text = name;
            std::cout << name << std::endl;
            cms::ContentManager cm(name);
            cms::ContentNode src;
            src.mutable_name() = "zeng";
            src.mutable_location() = "www.zeng.com";
            src.mutable_authors() = "zeng...";
            src.mutable_description() = "zeng zeng zeng";
            src.mutable_title() = "still zeng...";
            cm.CreateContentInfo(src);

            cms::ContentNode target;
            target.mutable_name() = src.name();
            target.mutable_location() = src.location();
            cm.GetContentInfo(target);
            assert(src == target);

            target.mutable_name().clear();
            cm.GetContentInfo(target);
            assert(! (target == src));

            src.mutable_name().clear();
            cm.UpdateContentInfo(src);
            cm.GetContentInfo(target);
            assert(src == target);

            cm.RemoveContent(src);
            assert(cm.GetContentInfo(target) == false);

            cm.DropAllTables();
        }

        // test 3. blob test.
        {
            /*
            using namespace boost::filesystem;
            cms::ContentManager cm("content.db");
            cms::ContentNode src;
            boost::progress_timer t;

            // Initialize thumbnail
            static const int IMAGE_SIZE = 100 * 100;
            char image_data[IMAGE_SIZE];
            src.mutable_thumbnail().size = IMAGE_SIZE;
            src.mutable_thumbnail().data = image_data;
            for(int i = 0; i < 10000; ++i)
            {
                std::stringstream ss;
                ss << i;
                src.mutable_name() = "image" + ss.str();
                src.mutable_location() = ss.str();
                cm.CreateContentInfo(src);
            }
            path location("content.db");

            // size: 102496256 100MB.
            // time: 93.468 (In my laptop)
            // sqlite seems does not occupy too many space.
            std::cout << file_size(location) << std::endl;
            std::cout << t.elapsed() << std::endl;
            cm.DropAllTables();
            */
        }

        // test 4. add category
        {
            using namespace cms;
            ContentManager cm("content.db");
            Category book_category;
            cm.CreateNewCategory(cm.mutable_local_root_category(),
                book_category);
            cms_ids children =
                cm.local_root_category().children_categories();

            cms_ids_iter iter = std::find(children.begin(), children.end(),
                book_category.id());
            assert (iter != children.end());
            cm.DropAllTables();
        }

        // test 5. remove category
        {
            using namespace cms;
            ContentManager cm("content.db");
            Category book_category, news_category;
            cm.CreateNewCategory(cm.mutable_local_root_category(),
                book_category);

            cm.CreateNewCategory(cm.mutable_local_root_category(),
                news_category);

            cm.RemoveChildCategory(cm.mutable_local_root_category(),
                book_category);

            cms_ids children = cm.local_root_category().children_categories();

            cms_ids_iter iter = std::find(children.begin(), children.end(),
                book_category.id());
            assert (iter == children.end());

            iter = std::find(children.begin(), children.end(),
                news_category.id());
            assert (iter != children.end());

            cm.DropAllTables();
        }

        // test 6. AddChildContent
        {
            using namespace cms;
            ContentManager cm("content.db");
            Category book_category;
            cm.CreateNewCategory(cm.mutable_local_root_category(),
                book_category);

            ContentNode content;
            content.mutable_name() = "book";
            content.mutable_location() = "www.book.com";
            cm.CreateContentInfo(content);

            cm.AddChildContent(book_category, content);
            cms_ids children = book_category.children_categories();

            cms_ids_iter iter = std::find(children.begin(), children.end(),
                content.id());
            assert(iter != children.end());
            cm.DropAllTables();
        }

        // test 7. RemoveChildContent
        {
            using namespace cms;
            ContentManager cm("content.db");
            Category book_category;
            cm.CreateNewCategory(cm.mutable_local_root_category(),
                book_category);

            ContentNode content;
            content.mutable_name() = "book";
            content.mutable_location() = "www.book.com";
            cm.CreateContentInfo(content);

            cm.AddChildContent(book_category, content);
            cms_ids children = book_category.children_categories();

            cms_ids_iter iter = std::find(children.begin(), children.end(),
                content.id());
            assert(iter != children.end());

            cm.RemoveChildContent(book_category, content);
            children = book_category.children_categories();

            iter = std::find(children.begin(), children.end(),
                content.id());
            assert(iter == children.end());
            cm.DropAllTables();
        }


    }
    catch(std::runtime_error const& err)
    {
        std::cerr << err.what();
        return -1;
    }
}
