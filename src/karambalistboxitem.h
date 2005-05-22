#include "karamba.h"
#include <qlistbox.h>

class KarambaListBoxItem : public QListBoxText 
{

public:
  QString appId; 
  KarambaListBoxItem( QListBox* listbox, const QString & text=QString::null );

};
