# Spec for building RunDSAMSim RPM Package

# Note that this is NOT a relocatable package
%include %{_rpmconfigdir}/macros.python

%define prefix	/usr
%define ver		2.1.9
%define RELEASE 1
%define rel		%{?CUSTOM_RELEASE} %{!?CUSTOM_RELEASE:%RELEASE}
%define	_libexecdir	%{prefix}/libexec/octave/dsam

Name: rundsamsim
Summary: rundsamsim function DLL for Octave/Matlab 
Version: %{ver}
Release: %{rel}
License: LICENSE 
Group: Applications/Education
Source: http://prdownloads.sourceforge.net/dsam/%{name}-%{ver}.tar.gz
URL: http://dsam.org.uk
Packager: Lowel P. O'Mard <lowel.omard@gmail.com> 
Requires: dsam java
BuildRoot: %{_tmppath}/%{name}-root
Obsoletes: rundsamsim

%description
The rundsamsim function DLL allows Matlab/Octave users run DSAM simulations as a
Matlab/Octave command.  It is backed up by the extensive collection of published and
new models supplied by the Development System for Auditory Modelling (DSAM).

%changelog
* Wed Jan 17 2006 Lowel P. O'Mard <lpo21@cam.ac.uk>
- Initial skeleton, based on the .specs ams-1.2.0

%prep
%setup
%configure --prefix=%{_libdir} --enable-java-support --enable-python-support --enable-generic-support

%build
if [ "$SMP" != "" ]; then
  export MAKE="make -j$SMP"
else
  export MAKE="make"
fi
$MAKE

%install
[ -n "$RPM_BUILD_ROOT" -a "$RPM_BUILD_ROOT" != / ] && rm -rf $RPM_BUILD_ROOT
rm -rf $RPM_BUILD_ROOT

%makeinstall

%post

%postun

%preun

%clean
[ -n "$RPM_BUILD_ROOT" -a "$RPM_BUILD_ROOT" != / ] && rm -rf $RPM_BUILD_ROOT

%files
%defattr(-, root, root)

%doc AUTHORS COPYING NEWS README.pdf README.rtf ChangeLog
%{_libexecdir}/*
%{_datadir}/java/*
%{_libdir}/*
%{py_sitedir}/*
%{_datadir}/%{name}/Examples/*

