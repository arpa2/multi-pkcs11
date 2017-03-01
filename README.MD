README for Multi PKCS #11 — Dæmon and Library
=============================================

> *The multiP11 dæmon is a program that accepts incoming connections over
> which the Remote PKCS #11 protocol is run.  The multiP11
> dæmon and library pass PKCS #11 calls on to customary
> PKCS #11 libraries, but not without some clever processing.*

The cleverness of this dæmon lies in its resolution of various problems
that make PKCS #11 a bit difficult in everyday practice:

  * **Library and Dæmon** are the two modes of operation of multiP11; the
    former is referred to as the *multiP11 library* and the latter as the
    *multiP11 dæmon*.

  * **Remote PKCS #11** is used to enable it as a hosted service, rather than
    a library that accesses local credentials.  This has the benefit of a
    shared credentials store, as well as offloading its protection to a
    safe site.  The safe site may be in one's home or at a solid service
    provider.

  * **Multiple encodings** are supported because Remote PKCS #11 is founded
    on ASN.1; normally it would use DER encoding, but XER and JER are both
    available as well, in support of web-based technology.

  * **Kerberos authentication** is used to authenticate the client application
    to the dæmon, and to encrypt the connection.  This is helpful for fast
    security bootstrapping, without falling back on fixed keys.  The two
    forms of Kerberos under consideration are TLS-KDH and GSS-API.

  * **SCTP transport** is an optional and suggested alternative to TCP.  It
    helps against head-of-line problems, because its delivery is reliable but
    not necessarily in-order.  The multiP11 dæmon sends its replies without
    any order (except for continuations of long blocks of data) and the
    client application may choose to let out-of-order delivery happen as a
    result of different applications (or their threads) accessing
    Remote PKCS #11 more than once at the same time.

  * **Authorisation** is supported by letting the client select a targeted
    resource (perhaps a group or a pseudonym) in the form of a remote slot/token.
    In addition or as a possible implementation, a PIN might be used to
    further select what information is made available to the client application,
    for instance by diversifying token implementations based on the PIN,
    or by applying rewrites or filters to them.  Authorisation may in fact
    permit access to tokens that have no implementation created yet;
    in such a case, a new one might be initialised on a free slot, as soon as
    one is available in a library.

  * **Layered PKCS #11** supports a combined PKCS #11 resource that may be
    composed of individual underlying PKCS #11 layers.  This supports the
    sharing of such layers between group members, for example.

  * **PKCS #11 NAT** is used for Layered PKCS #11, but it can also be used
    to enable more than one PKCS #11 repository at the same time to
    client applications that can deal with only one (but that cause headaches
    in transitional periods).

  * **PKCS #11 Split & Share** splits various backend PKCS #11 libraries
    in a way uncommon for client applications that accept multilpe libraries
    at the same time.  Specifically, a shared memory segment holding
    PKCS #11 data and synchronisation primitives is allocated between
    the multiP11 dæmon and each individual PKCS #11 backend library; the backends
    run in isolated processes and cannot access each other's shared memory.
    The use of this is to forego rogue PKCS #11 libraries, and thereby make
    it easier to install a new PKCS #11 library at a hosting customer's request.
    The operating system is assumed to have encrypted swap, of course.

