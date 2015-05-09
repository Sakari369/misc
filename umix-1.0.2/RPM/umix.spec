# $Id: umix.spec,v 1.6 2003/08/30 15:33:57 sakari Exp $
#
# RPM spec file for Umix
#
# Copyright (C) 2002 Sakari Lehtonen <sakari@ionstream.fi>
#
%define name	umix
%define version	1.0.2
%define release	1

Summary:	Program for adjusting soundcard mixers. 
Name:		%{name}
Version:	%{version}
Release:	%{release}
Copyright:	GPL
Url:		http://umix.sf.net/
Group:		Applications/Multimedia
Source:		%{name}-%{version}.tar.gz
Prefix:		%{_prefix}
BuildRoot:	/var/tmp/%{name}-%{version}-root

%description
Umix is a program for adjusting soundcard volumes and other
features in soundcard mixers. You can control your volumes, balances and
recording sources flexibly from the command line or with a ncurses
user interface with familiar vi/emacs keybindings. Umix supports multiple 
mixer devices. All settings can be saved and loaded from a file.

%prep
%setup -q

%build
./configure --prefix=%{_prefix}
make

%install
rm -rf $RPM_BUILD_ROOT
mkdir -p $RPM_BUILD_ROOT/%{_prefix}/bin
mkdir -p $RPM_BUILD_ROOT/etc
mkdir -p $RPM_BUILD_ROOT/%{_prefix}/man/man1

make prefix=$RPM_BUILD_ROOT%{_prefix} install

strip $RPM_BUILD_ROOT/%{_prefix}/bin/umix

%clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(-,root,root)
%doc AUTHORS ChangeLog NEWS README TODO
%{_prefix}/bin/umix
%{_prefix}/man/man1/umix.1*

%changelog
* Sat Aug 30 2003 Sakari Lehtonen <sakari@ionstream.fi>
- New upstream version 1.0.2
* Sat Aug 16 2003 Sakari Lehtonen <sakari@ionstream.fi>
- New upstream version 1.0.1
* Sat Feb 08 2003 Sakari Lehtonen <sakari@ionstream.fi>
- Version 1.0, this is the final release
- Removed plugin stuff
- Added %define's to ease the updating of this spec
- Updated the URL point to Sourceforge.net
* Mon Sep 16 2002 Sakari Lehtonen <sakari@ionstream.fi>
- New upstream version 0.9.2
- Changed download URL
* Sat Jul 13 2002 Sakari Lehtonen <sakari@ionstream.fi>
- New upstream version 0.9.1
- Updated description
* Sat Jun 08 2002 Sakari Lehtonen <sakari@ionstream.fi>
- New upstream version 0.9.0
- Fixed a typo in the previous release date ;)
* Thu Feb 21 2002 Sakari Lehtonen <sakari@ionstream.fi>
- New upstream version 0.8.3
* Fri Nov 30 2001 Sakari Lehtonen <sakari@ionstream.fi>
- New upstream version 0.8.2
- Updated description
* Tue Nov 13 2001 Sakari Lehtonen <sakari@ionstream.fi>
- New upstream version 0.8.1
* Mon Nov 05 2001 Sakari Lehtonen <sakari@ionstream.fi>
- New upstream version 0.8.0
* Sat Jul 21 2001 Sakari Lehtonen <sakari@ionstream.fi>
- New upstream version 0.7.2
* Wed Jun 23 2001 Sakari Lehtonen <sakari@ionstream.fi>
- New upstream version 0.7.1
* Tue Jun 08 2001 Sakari Lehtonen <sakari@ionstream.fi>
- New upstream version 0.7.0
- Changed description and installing of libraries
* Mon Mar 19 2001 Sakari Lehtonen <sakari@ionstream.fi>
- New upstream version 0.6.0
* Thu Mar 08 2001 Sakari Lehtonen <sakari@ionstream.fi>
- New upstream version 0.5.1
* Mon Feb 19 2001 Sakari Lehtonen <sakari@ionstream.fi>
- New upstream version 0.5
* Sun Jan 28 2001 Sakari Lehtonen <sakari@ionstream.fi>
- New upstream version 0.4
* Tue Jan 23 2001 Sakari Lehtonen <sakari@ionstream.fi>
- New upstream version 0.3
* Mon Jan 08 2001 Sakari Lehtonen <sakari@ionstream.fi>
- Initial RPM package
