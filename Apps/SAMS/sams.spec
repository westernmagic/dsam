# Spec for building DSAM RPM Packages

# Note that this is NOT a relocatable package
%define prefix	/usr
%define ver		1.3.10
%define RELEASE 0
%define rel		%{?CUSTOM_RELEASE} %{!?CUSTOM_RELEASE:%RELEASE}
%define	desktop_vendor	 cnbh
%define desktop_utils    %(if which desktop-file-install >/dev/null 2>&1; then echo "yes" ; fi)

Name: sams
Summary: Auditory Model Simulator 
Version: %{ver}
Release: %{rel}
License: LICENSE 
Group: Applications/Education
Source: http://prdownloads.sourceforge.net/dsam//%{name}-%{ver}.tar.gz
URL: http://dsam.org.uk
Packager: Lowel P. O'Mard <lowel.omard@gmail.com> 
Requires: dsam
BuildRoot: %{_tmppath}/%{name}-root
Obsoletes: sams

%description
The Auditory Model Simulator (SAMS) allows users to design, run and evaluate
auditory models.  It is supported by the extensive collection of published and
new models supplied by the Development System for Auditory Modelling (DSAM).

%changelog
* Tue Dec 02 2008 Lowel P. O'Mard <lowel@cam.ac.uk> - 1:1.2.24-1
- Updated spec file.  Tested on Fedora Core 10.

* Wed Mar 31 2000 Lowel P. O'Mard <lowel@essex.ac.uk>
- Initial skeleton, based on the .specs dsam-2.2.25

%prep
%setup
SAMS_FLAGS="--prefix=%{prefix}"
%configure $SAMS_FLAGS

%build
# Needed for snapshot releases.
if [ "$SMP" != "" ]; then
  export MAKE="make -j$SMP"
else
  export MAKE="make"
fi
$MAKE

%install
[ -n "$RPM_BUILD_ROOT" -a "$RPM_BUILD_ROOT" != / ] && rm -rf $RPM_BUILD_ROOT
rm -rf $RPM_BUILD_ROOT

# desktop entry
cat << EOF > %{desktop_vendor}-%{name}.desktop
[Desktop Entry]
Name=SAMS
Comment=Auditory Model Simulator
Icon=sams.xpm
Exec=%{name}
Terminal=false
Type=Application
StartupNotify=true
EOF
%if "%{desktop_utils}" == "yes"
  mkdir -p $RPM_BUILD_ROOT%{_datadir}/applications
  desktop-file-install --delete-original \
	--vendor %{desktop_vendor} \
    --dir $RPM_BUILD_ROOT%{_datadir}/applications    \
    --add-category X-Red-Hat-Base                 \
    --add-category Application                    \
    --add-category Education                     \
    %{desktop_vendor}-%{name}.desktop
%else
  install -d $RPM_BUILD_ROOT%{_sysconfdir}/X11/applnk/Scientific
  install %{desktop_vendor}-%{name}.desktop $RPM_BUILD_ROOT%{_sysconfdir}/X11/applnk/Scientific/
%endif

mkdir -p $RPM_BUILD_ROOT/etc/xinetd.d
cat <<EOF > $RPM_BUILD_ROOT/etc/xinetd.d/%{name}_server
# default: on
# description: The DSAM %{name} application runs in server mode.
service %{name}_server
{
	type		= UNLISTED
	disable		= no
	port		= 3300
	socket_type	= stream
	protocol	= tcp
	wait		= no
	user		= root
	server		= %{_bindir}/%{name}_ng
	server_args	= -S -S
}
EOF

%makeinstall

%post
%if "%{desktop_utils}" == "yes"
	update-desktop-database -q
%endif

%postun

%preun

%clean
[ -n "$RPM_BUILD_ROOT" -a "$RPM_BUILD_ROOT" != / ] && rm -rf $RPM_BUILD_ROOT

%files
%defattr(-, root, root)

%doc AUTHORS COPYING INSTALL README.txt NEWS ChangeLog
%{_datadir}/%{name}/DEFAULT/*
%{_datadir}/%{name}/HelpFiles/*
%{_datadir}/%{name}/Tutorials/*
%{_datadir}/%{name}/Matlab/*
%{_datadir}/%{name}/Octave/*
%{_bindir}/*
%{_mandir}/*
%{_datadir}/pixmaps/*
/etc/xinetd.d/*
%if "%{desktop_utils}" == "yes"
%{_datadir}/applications/*%{name}.desktop
%else
%{_sysconfdir}/X11/applnk/Scientific/%{desktop_vendor}-%{name}.desktop
%endif

