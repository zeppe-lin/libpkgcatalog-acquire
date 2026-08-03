# ELF ABI policy

The first independent public ABI is `libpkgcatalog-acquire.so.3`. The major
matches the catalog/source-3 authority generation carried by public value types.
It does not imply that the earlier in-tree acquisition library had three
independent releases.

The reviewed symbols are listed in `abi/libpkgcatalog-acquire.exports` and are
applied through a linker version script. Hidden-by-default compilation and the
manifest together prevent filesystem helpers, YAML-provider symbols, and
standard-library template instantiations from becoming accidental ABI.

`pkgcatalog::acquire::error` exports its destructor, RTTI, type name, and vtable
so downstream code can catch it across the shared-library boundary.

Public headers expose `libpkgcatalog` and transitive source authority types.
They expose no `libpkgsource-yaml` or libyaml types. Pkg-config therefore places
`libpkgcatalog >= 3.0.0` in `Requires` and keeps `libpkgsource-yaml >= 1.0.0` in
`Requires.private`.

Any public declaration, object representation, enum value, exception hierarchy,
SONAME, or dependency-placement change requires explicit ABI review and an
updated manifest when exported symbols change.
