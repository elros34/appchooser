Name:       appchooser


Summary:    Application chooser
Version:    0.1.2
Release:    1
Group:      Qt/Qt
License:    LICENSE
URL:        http://example.org/
Source0:    %{name}-%{version}.tar.bz2
Requires:   sailfishsilica-qt5 >= 0.10.9
Requires:   awk
BuildRequires:  pkgconfig(sailfishapp) >= 1.0.2
BuildRequires:  pkgconfig(Qt5Core)
BuildRequires:  pkgconfig(Qt5Qml)
BuildRequires:  pkgconfig(Qt5Quick)
BuildRequires:  pkgconfig(Qt5Xml)
BuildRequires:  desktop-file-utils
BuildRequires:  pkgconfig(contentaction5)
BuildRequires:  pkgconfig(mlite5)
BuildRequires:  pkgconfig(nemonotifications-qt5)

%description
Application chooser


%prep
%setup -q -n %{name}-%{version}


%build

%qmake5 

make %{?_smp_mflags}


%install
rm -rf %{buildroot}
%qmake5_install

desktop-file-install --delete-original       \
  --dir %{buildroot}%{_datadir}/applications             \
   %{buildroot}%{_datadir}/applications/*.desktop

%post

[ -z $LOGNAME ] && LOGNAME="$(loginctl --no-legend list-users | awk '{print $2}' | head -n1)"

# maybe move it to app itself and use qdevel-su

# fix webcat
if [ -f /home/$LOGNAME/.local/share/applications/mimeinfo.cache ]; then
    echo "Fixing webcat"
    rm /home/$LOGNAME/.local/share/applications/mimeinfo.cache
    mv /home/$LOGNAME/.local/share/applications/harbour-webcat-open-url.desktop /usr/share/applications/ || true
fi

if [ "$(su $LOGNAME -c "dconf read /apps/appchooser/mimeappsVersion")" != "1" ]; then
    echo "Setting AppChooser as main MIME handler for $LOGNAME"
    su $LOGNAME -c "dconf write /apps/appchooser/mimeappsVersion 1"
    mv /home/$LOGNAME/.local/share/applications/mimeapps.list /home/$LOGNAME/.local/share/applications/mimeapps.list.appchooser_bac || true
    cp -a %{_datadir}/%{name}/mimeapps-appchooser.list /home/$LOGNAME/.local/share/applications/mimeapps.list
    chown $LOGNAME:$LOGNAME /home/$LOGNAME/.local/share/applications/mimeapps.list
fi

killall appchooser 2>/dev/null || true
update-desktop-database 2>&1 | grep -v x-maemo-highlight || true

# Remove OpenFileDialog.qml to allow pre 3.1.0 release way to open MIME handlers
echo "Backup OpenFileDialog.qml"
mv %{_datadir}/lipstick-jolla-home-qt5/launcher/OpenFileDialog.qml %{_datadir}/lipstick-jolla-home-qt5/launcher/OpenFileDialog.qml.appchooser_bac || true

%postun
if [ $1 == 0 ]; then
    echo "Resetting MIME handlers"
    su $LOGNAME -c "dconf reset /apps/appchooser/mimeappsVersion"
    rm /home/$LOGNAME/.local/share/applications/mimeapps.list || true
    update-desktop-database 2>&1 | grep -v x-maemo-highlight
    echo "Restoring OpenFileDialog.qml"
    mv %{_datadir}/lipstick-jolla-home-qt5/launcher/OpenFileDialog.qml.appchooser_bac %{_datadir}/lipstick-jolla-home-qt5/launcher/OpenFileDialog.qml
fi

%files
%defattr(-,root,root,-)
%{_bindir}
%{_datadir}/%{name}
%{_datadir}/jolla-settings/entries/%{name}-settings.json
%{_libdir}/qt5/qml/AppChooser/Settings/libappchooser-settings.so
%{_libdir}/qt5/qml/AppChooser/Settings/qmldir
%{_datadir}/applications/%{name}.desktop
%{_datadir}/icons/hicolor/*/apps/%{name}.png
%{_datadir}/dbus-1/services
%{_datadir}/contentaction/*.xml
%attr(0666, root, root) %{_datadir}/%{name}/mimeapps-appchooser.list
