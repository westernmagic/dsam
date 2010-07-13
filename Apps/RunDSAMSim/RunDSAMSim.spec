# Spec for building RunDSAMSim RPM Package

# Note that this is NOT a relocatable package
%define prefix	/usr
%define ver		1.1.9
%define RELEASE 1
%define rel		%{?CUSTOM_RELEASE} %{!?CUSTOM_RELEASE:%RELEASE}
%define	_libexecdir	%{prefix}/libexec/octave/dsam

Name: rundsamsim
Summary: rundsamsim function DLL for Octave/Matlab 
Version: %{ver}
Release: %{rel}
License: LICENSE 
Group: Applications/Education
Source: http://www.pdn.cam.ac.uk/groups/dsam/Downloads/Release/%{name}-%{ver}.tar.gz
URL: http://dsam.org.uk
Packager: Lowel P. O'Mard <lpo21@cam.ac.uk> 
Requires: dsam
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
%configure

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

%doc AUTHORS COPYING README.txt NEWS ChangeLog
%{_libexecdir}/*

