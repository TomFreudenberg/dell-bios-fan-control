%define debug_package %{nil}

Summary: Disable/Enable BIOS fan control on  Dell laptops
Name: {{{ git_name }}}
Version: {{{ git_version lead=0 follow=01 }}}
Release: 1%{?dist}
License: GPLv2+
Source: {{{ git_pack }}}
URL: https://github.com/TomFreudenberg/dell-bios-fan-control
Requires: ld-linux.so.2 libc.so.6
Provides: %{name}-%{version}-%{release}

%description
A tool that enables/disables the BIOS fan control on some Dell Laptops. This is required on some Notebooks to be able to control fanspeed with i8kmon, without the BIOS immediately overruling it again.

%prep
rm -rf %buildroot
{{{ git_setup_macro }}}

%build
make

%install
mkdir -p $RPM_BUILD_ROOT/%{_sbindir}
cp dell-bios-fan-control $RPM_BUILD_ROOT/%{_sbindir}/

%clean
make clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(-, root, root)
%{_sbindir}/dell-bios-fan-control

%changelog
* Sun Jun 3 2018 uriesk <uriesk@posteo.de> 1.43
- created spec file
