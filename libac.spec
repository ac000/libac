Name:		libac
Version:	0.2.0
Release:	1%{?dist}
Summary:	Library of miscellaneous utility functions

Group:		System Environment/Libraries
License:	LGPLv2.1
URL:		http://github.com/ac000/libac
Source0:	libac-%{version}.tar
BuildRoot:	%(mktemp -ud %{_tmppath}/%{name}-%{version}-%{release}-XXXXXX)

BuildRequires:	glibc-devel

%description
libac is a library containing miscellaneous utility functions.


%prep
%setup -q


%build
make -C src/ %{?_smp_mflags}


%install
rm -rf $RPM_BUILD_ROOT
install -Dp -m644 src/include/libac.h $RPM_BUILD_ROOT/%{_includedir}/libac.h
install -Dp -m0755 src/libac.so.%{version} $RPM_BUILD_ROOT/%{_libdir}/libac.so.%{version}
cd $RPM_BUILD_ROOT/%{_libdir}
ln -s libac.so.%{version} libac.so.0.2
ln -s libac.so.0.2 libac.so
cd -

%post -p /sbin/ldconfig

%clean
rm -rf $RPM_BUILD_ROOT


%files
%defattr(-,root,root,-)
%doc README.md COPYING
%{_libdir}/libac.*
%{_includedir}/libac.h


%changelog

* Thu Apr 13 2017 Andrew Clayton <andrew@digital-domain.net> - 0.2.0-1
- Added a couple of time related functions
- Version v0.2.0

* Thu Apr 13 2017 Andrew Clayton <andrew@digital-domain.net> - 0.1.0-1
- Added binary tree functions
- Version v0.1.0

* Tue Apr 11 2017 Andrew Clayton <andrew@digital-domain.net> - 0.0.1-1
- Initial version.
