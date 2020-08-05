// Tcl-specific SWIG directives
%module dbxml_tcl
%{
extern "C" void *_NameToPtr(const char *);
#define NAME_TO_ENV(name) (DB_ENV *)_NameToPtr((name))
#define NAME_TO_TXN(name) (DB_TXN *)_NameToPtr((name))
%}

%typemap(in) const unsigned char * {
  int len = 0;
  char *str = Tcl_GetStringFromObj($input, &len);
  if (len)
      $1 = (unsigned char *) str;
  else
      $1 = 0;
}

%typemap(out) u_int32_t * {
	for (int i = 0; $1[i]; i++)
		Tcl_ListObjAppendElement(interp, $result, Tcl_NewIntObj($1[i]));
	delete [] $1;
}

%typemap(in) DB_ENV * {
	char *arg = Tcl_GetStringFromObj($input, NULL);
	DB_ENV *envp = NAME_TO_ENV(arg);
	$1 = envp;
}

%typemap(in) DB_TXN * {
	char *arg = Tcl_GetStringFromObj($input, NULL);
	DB_TXN *txnp = NAME_TO_TXN(arg);
	$1 = txnp;
}

%typemap(in) XmlData *, XmlData & (XmlData xml_data) {
	int len;
	char *data = (char *)Tcl_GetByteArrayFromObj($input, &len);
  xml_data.set(data, len);
	$1 = &xml_data;
}

%typemap(out) const XmlData *, XmlData * {
	if ($1) {
		Tcl_SetObjResult(interp, Tcl_NewByteArrayObj((unsigned char *)$1->get_data(), $1->get_size()));
		delete $1; // done with new XmlData object
	} else
		Tcl_SetResult(interp, (char *) "", TCL_STATIC);
}

%typemap(in) const void * {
	int len;
	char *data = (char *)Tcl_GetByteArrayFromObj($input, &len);
	$1 = data;
}

%typemap(in) void * {
	int len;
	char *data = (char *)Tcl_GetByteArrayFromObj($input, &len);
	$1 = data;
}

/* 
 * gmf: in order to support function overloads that use DB_TXN and DB_ENV,
 * which is the subject of a typemap(in), above, a typemap(typecheck)
 * is required; otherwise, the generated code fails.  This is a general
 * rule for SWIG and overloads.  This check always succeeds.  Precedence is
 * set high (low).
 */
%typemap(typecheck, precedence=2000) DB_TXN * {
 /* This would be a typecheck for a DB_TXN *; just succeed */
 $1 = 1; 
}

%typemap(typecheck, precedence=2000) DB_ENV * {
 /* This would be a typecheck for a DB_ENV *; just succeed */
 $1 = 1; 
}


%exception {
	try {
		$function
	} catch (XmlException &e) {
		Tcl_SetResult(interp, (char *)e.what(), TCL_VOLATILE);
		goto fail;
	}
}
