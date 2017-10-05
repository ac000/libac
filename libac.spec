Name:		libac
Version:	0.22.1
Release:	1%{?dist}
Summary:	Library of miscellaneous utility functions

Group:		System Environment/Libraries
License:	LGPLv2.1
URL:		https://github.com/ac000/libac
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
ln -s libac.so.0 libac.so
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

* Thu Oct 5 2017 Andrew Clayton <andrew@digital-domain.net> - 0.22.1-1
- Include inttypes.h in libac.h

* Wed Oct 4 2017 Andrew Clayton <andrew@digital-domain.net> - 0.22.0-1
- Allow the circular queue to overwrite when full

* Sun Oct 1 2017 Andrew Clayton <andrew@digital-domain.net> - 0.21.0-1
- Allow the circular queue to dynamically grow in size

* Fri Aug 11 2017 Andrew Clayton <andrew@digital-domain.net> - 0.20.1-1
- Brown paper bag release fixing rpm building.

* Fri Aug 11 2017 Andrew Clayton <andrew@digital-domain.net> - 0.20.0-1
- Added AC_STR_SPLIT_ALWAYS #define

* Sun Jul 23 2017 Andrew Clayton <andrew@digital-domain.net> - 0.19.0-1
- Enhancement to ac_str_split()

* Tue Jul 18 2017 Andrew Clayton <andrew@digital-domain.net> - 0.18.0-1
- Add a string split function.

* Wed Jul 5 2017 Andrew Clayton <andrew@digital-domain.net> - 0.17.0-1
- Add a couple of functions to ac_net: ac_net_ipv4_isin & ac_net_ipv6_isin

* Thu Jun 22 2017 Andrew Clayton <andrew@digital-domain.net> - 0.16.0-1
- Add a function to return the number of entries in an ac_slist_t
- Build fix's for older versions of glibc & GCC such as with CentOS 6/7

* Tue Jun 13 2017 Andrew Clayton <andrew@digital-domain.net> - 0.15.0-1
- Add some aliases for __attribute__((unused))

* Mon Jun 5 2017 Andrew Clayton <andrew@digital-domain.net> - 0.14.0-1
- Compile with -fvisibility=hidden

* Tue May 30 2017 Andrew Clayton <andrew@digital-domain.net> - 0.13.1-1
- Fix a segfault in ac_slist_destroy()

* Tue May 23 2017 Andrew Clayton <andrew@digital-domain.net> - 0.13.0-1
- Add some more network related functions; ac_net_inet_pton(),
  ac_net_inet_ntop() & ac_net_port_from_sa()

* Fri May 19 2017 Andrew Clayton <andrew@digital-domain.net> - 0.12.0-1
- Add a couple more fucntions to ac_slist
- Add more API details to the README

* Tue May 16 2017 Andrew Clayton <andrew@digital-domain.net> - 0.11.0-1
- Add a password hasher function, wrapper around crypt_r(3)
- Compile with -Wextra

* Mon May 8 2017 Andrew Clayton <andrew@digital-domain.net> - 0.10.1-1
- Enhancements to ac_fs_copy()

* Sat May 6 2017 Andrew Clayton <andrew@digital-domain.net> - 0.10.0-2
- Change the library soname

* Fri May 5 2017 Andrew Clayton <andrew@digital-domain.net> - 0.10.0-1
- Add some geospatial functions
- Version 0.10.0

* Sun Apr 30 2017 Andrew Clayton <andrew@digital-domain.net> - 0.9.0-1
- Add a file copy function
- Version 0.9.0

* Sat Apr 29 2017 Andrew Clayton <andrew@digital-domain.net> - 0.8.0-1
- Add some singly linked list functions
- Version 0.8.0

* Thu Apr 27 2017 Andrew Clayton <andrew@digital-domain.net> - 0.7.1-1
- Make ac_misc_ppb() thread safe
- Version 0.7.1

* Wed Apr 26 2017 Andrew Clayton <andrew@digital-domain.net> - 0.7.0-1
- Add some quark (string to integer mapping) functions
- Add a Levenshtein distance function
- Version 0.7.0

* Wed Apr 26 2017 Andrew Clayton <andrew@digital-domain.net> - 0.6.1-1
- Fix a couple of possible segfaults
- Version v0.6.1

* Wed Apr 19 2017 Andrew Clayton <andrew@digital-domain.net> - 0.6.0-1
- Added a simple circular queue implementation
- Version v0.6.0

* Tue Apr 18 2017 Andrew Clayton <andrew@digital-domain.net> - 0.5.0-1
- Added a couple of network name lookup functions
- Version v0.5.0

* Sat Apr 15 2017 Andrew Clayton <andrew@digital-domain.net> - 0.4.0-1
- Added a print pretty bytes function
- Version v0.4.0

* Fri Apr 14 2017 Andrew Clayton <andrew@digital-domain.net> - 0.3.1-1
- Fix a memory leak in ac_fs_mkdir_p()
- Allow to create relative directory paths in ac_fs_mkdir_p()
- Version v0.3.1

* Thu Apr 13 2017 Andrew Clayton <andrew@digital-domain.net> - 0.3.0-1
- Added a couple of filesystem related functions.
- Version v0.3.0

* Thu Apr 13 2017 Andrew Clayton <andrew@digital-domain.net> - 0.2.0-1
- Added a couple of time related functions
- Version v0.2.0

* Thu Apr 13 2017 Andrew Clayton <andrew@digital-domain.net> - 0.1.0-1
- Added binary tree functions
- Version v0.1.0

* Tue Apr 11 2017 Andrew Clayton <andrew@digital-domain.net> - 0.0.1-1
- Initial version.
