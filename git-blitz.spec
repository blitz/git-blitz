%global git_hash d1cc2800646436f47b1008f5221ffe8ebad9c84e

Summary:	Julian\'s personal git automation tool
Name:           git-blitz
Version:	0.0
Release:	1%{?dist}
Group:		Development/Tools
License:	GPLv2
URL:		https://github.com/blitz/%{name}
Source0:	https://github.com/blitz/%{name}/archive/%{git_hash}.tar.gz

BuildRequires:	scons
Requires:	boost-devel
BuildRequires:	libgit2-devel
Requires:	boost-program-options
Requires:	libgit2

%description
A set of tools to automate Julian\'s particular development workflow. YMMV.

%prep
%autosetup -n %{name}-%{git_hash}

%build
scons %{?_smp_mflags}

%install
install -m 755 -d %{buildroot}/%{_bindir}
install -s -m 755 git-blitz %{buildroot}/%{_bindir}

%files
%{_bindir}/git-blitz

%doc

%changelog
