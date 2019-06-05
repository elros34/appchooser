Name:       appchooser


Summary:    Application chooser
Version:    0.0.1
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
su -l nemo -c "lca-tool --setmimedefault x-scheme-handler/http appchooser"
su -l nemo -c "lca-tool --setmimedefault x-scheme-handler/https appchooser"
killall appchooser || true

%postun
su -l nemo -c "lca-tool --resetmimedefault x-scheme-handler/http"
su -l nemo -c "lca-tool --resetmimedefault x-scheme-handler/https"


%files
%defattr(-,root,root,-)
%{_bindir}
%{_datadir}/%{name}
%{_datadir}/applications/%{name}.desktop
%{_datadir}/icons/hicolor/*/apps/%{name}.png
%{_datadir}/dbus-1/services
