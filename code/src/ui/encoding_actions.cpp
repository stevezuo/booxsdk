#include "onyx/ui/encoding_actions.h"

namespace ui
{

static const base::string ENCODINGS[] =
{
    "UTF-8",
    "UTF-7",
    "UTF-16",
    "UTF-16BE",
    "GB18030",
    "Big5",
    "ISO-8859-1",
    "ISO-8859-2",
    "windows-1250",
    "windows-1251",
    "EUC-JP",
    "ISO-2022-JP",
    "Shift_JIS",
};
static const int ENCODINGS_SIZE = sizeof(ENCODINGS)/sizeof(ENCODINGS[0]);

static int caseCmp(const char *s1, const char *s2)
{
#ifdef _WINDOWS
    return _stricmp(s1, s2);
#else
    return strcasecmp(s1, s2);
#endif
}


EncodingActions::EncodingActions()
    : BaseActions()
{
    category()->setIcon(QIcon(QPixmap(":/images/encoding.png")));
}

EncodingActions::~EncodingActions(void)
{
}

void EncodingActions::generateActions(const base::string &encoding)
{
    category()->setText(QCoreApplication::tr("Encoding"));
    actions_.clear();

    for(int i = 0; i < ENCODINGS_SIZE; ++i)
    {
        // Add to category automatically.
        shared_ptr<QAction> act(new QAction(ENCODINGS[i].c_str(), exclusiveGroup()));
        act->setCheckable(true);
        act->setData(i);
        if (caseCmp(encoding.c_str(), ENCODINGS[i].c_str()) == 0)
        {
            act->setChecked(true);
        }
        // TODO, maybe need more icons.
        act->setIcon(QIcon(QPixmap(":/images/encoding.png")));
        actions_.push_back(act);
    }
}

base::string EncodingActions::encoding()
{
    // Search for the changed actions.
    QAction * act = exclusiveGroup()->checkedAction();
    if (act)
    {
        bool ok = false;
        int index = act->data().toInt(&ok);
        if (ok)
        {
            return ENCODINGS[index];
        }
    }
    return "";
}


}   // namespace ui

