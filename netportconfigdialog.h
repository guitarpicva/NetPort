// ----------------------------------------------------------------------------
// Copyright (C) 2016-2024 Mitch Winkle
// ALL RIGHTS RESERVED
//
// This file is part of the NetPort software project.  NetPort is
// NOT free software.  It has been released to the members of the Military
// Auxiliary Radio System organization under no-cost terms.  That release
// grant may be rescinded at any time by the author.
//
// NetPort is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//
// Parts of NetPort may contain source code from Open Source projects.
// See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
// ----------------------------------------------------------------------------
#ifndef NETPORTCONFIGDIALOG_H
#define NETPORTCONFIGDIALOG_H

#include <QDialog>

namespace Ui {
class NetPortConfigDialog;
}

class NetPortConfigDialog : public QDialog
{
    Q_OBJECT
signals:
    void settingsSaved();
public:
    explicit NetPortConfigDialog(QString name, QWidget *parent = 0);
    ~NetPortConfigDialog();

private slots:
    void on_saveButton_clicked();
    void on_cancelButton_clicked();

private:
    Ui::NetPortConfigDialog *ui;
    QString npname;
    void saveSettings();
    void loadSettings();
};

#endif // NETPORTCONFIGDIALOG_H
