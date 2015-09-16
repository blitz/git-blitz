%global git_hash ef1131cf6d5f15905e4d1d4c97adf58d5fd4550c

Summary:        Julian\'s personal git automation tool
Name:           git-blitz
Version:        0.0
Release:        1%{?dist}
Group:          Development/Tools
License:        GPLv2
URL:            https://github.com/blitz/%{name}
Source0:        https://github.com/blitz/%{name}/archive/%{git_hash}.tar.gz

BuildRequires:  scons
BuildRequires:  boost-devel
BuildRequires:  libgit2-devel
BuildRequires:  git
Requires:       boost-program-options
Requires:       libgit2

%description
A set of tools to automate Julian\'s particular development workflow. YMMV.

%prep
%autosetup -n %{name}-%{git_hash}

%build
scons %{?_smp_mflags}

%install
install -m 755 -d %{buildroot}/%{_bindir}
install -m 755 git-blitz %{buildroot}/%{_bindir}

%files
%{_bindir}/git-blitz

%doc

%changelog
