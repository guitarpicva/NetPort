// ----------------------------------------------------------------------------
// Copyright (C) 2016 Mitch Winkle
// ALL RIGHTS RESERVED
//
// This file is part of Station Manager software project.  Station Manager is
// NOT free software.  It has been released to the members of the Military
// Auxiliary Radio System organization under no-cost terms.  That release
// grant may be rescinded at any time by the author.
//
// Station Manager is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//
// Parts of Station Manager may contain source code from Open Source projects.
// See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
// ----------------------------------------------------------------------------
#include "netport.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    NetPort w("NetPort.ini");
    w.show();

    return a.exec();
}
