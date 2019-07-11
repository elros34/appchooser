Name:       appchooser


Summary:    Application chooser
Version:    0.0.4
Release:    1
Group:      Qt/Qt
License:    LICENSE
URL:        http://example.org/
Source0:    %{name}-%{version}.tar.bz2
Requires:   sailfishsilica-qt5 >= 0.10.9
BuildRequires:  pkgconfig(sailfishapp) >= 1.0.2
BuildRequires:  pkgconfig(Qt5Core)
BuildRequires:  pkgconfig(Qt5Qml)
BuildRequires:  pkgconfig(Qt5Quick)
BuildRequires:  desktop-file-utils
BuildRequires:  pkgconfig(contentaction5)
BuildRequires:  pkgconfig(mlite5)

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

# fix webcat
if [ -f /home/nemo/.local/share/applications/mimeinfo.cache ]; then
    echo "Fixing webcat"
    rm /home/nemo/.local/share/applications/mimeinfo.cache
    mv /home/nemo/.local/share/applications/harbour-webcat-open-url.desktop /usr/share/applications/
fi

if [ "$(su nemo -c "dconf read /apps/appchooser/domination")" != "true" ]; then
    echo "Setting AppChooser as main MIME handler"
    su nemo -c "dconf write /apps/appchooser/domination true"
    mv /home/nemo/.local/share/applications/mimeapps.list /home/nemo/.local/share/applications/mimeapps.list.bac
    cp -a %{_datadir}/%{name}/mimeapps-appchooser.list /home/nemo/.local/share/applications/mimeapps.list
fi

killall appchooser 2>/dev/null || true
update-desktop-database 2>&1 | grep -v x-maemo-highlight

%postun
if [ $1 == 0 ]; then
    echo "Resetting MIME handlers"
    su nemo -c "dconf reset /apps/appchooser/domination"
    rm /home/nemo/.local/share/applications/mimeapps.list
    update-desktop-database 2>&1 | grep -v x-maemo-highlight
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
%attr(0666, nemo, nemo) %{_datadir}/%{name}/mimeapps-appchooser.list
