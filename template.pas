{
@abstract( unit)

The unit is a part of Template Algorithms and Collections Library and contains related constants

@author(George Bakhtadze (avagames@gmail.com))
}

unit template;
{$I g3config.inc}

interface
  //  Template option constants
  const
    // sort in descending order
    soDescending = 0;
    // sort data can be extremely quicksort-unfriendly
    soBadData = 1;

  type
    // Data structure options set elements
    TDataStructureOption = (// data structure value can be nil
                            dsNullable,
                            // data structure should perform range checking
                            dsRangeCheck
                            // data structure key is a string (to correctly select hash function)
                            //dsStringKey
                            );

    // Type for collection indexes, sizes etc
    __CollectionIndexType = Integer;

    TCollection = interface

    end;

    TTplList = interface

    end;

    TMap = interface

    end;

    // Implements base interface for template classes
    TTemplateInterface = class(TObject, IInterface)
    protected
      function QueryInterface({$IFDEF FPC_HAS_CONSTREF}constref{$ELSE}const{$ENDIF} IID: TGUID; out Obj): HResult;
      {$IFDEF FPC}
      {$IF (not defined(WINDOWS)) AND (FPC_FULLVERSION>=20501)}cdecl{$ELSE}stdcall{$IFEND}
      {$ELSE}stdcall{$ENDIF};
      function _AddRef:  Integer;
      {$IFDEF FPC}
      {$IF (not defined(WINDOWS)) AND (FPC_FULLVERSION>=20501)}cdecl{$ELSE}stdcall{$IFEND}
      {$ELSE}stdcall{$ENDIF};
      function _Release: Integer;
      {$IFDEF FPC}
      {$IF (not defined(WINDOWS)) AND (FPC_FULLVERSION>=20501)}cdecl{$ELSE}stdcall{$IFEND}
      {$ELSE}stdcall{$ENDIF};
    end;

implementation

{ TTemplateInterface }

function TTemplateInterface.QueryInterface({$IFDEF FPC_HAS_CONSTREF}constref{$ELSE}const{$ENDIF} IID: TGUID; out Obj): HResult;
{$IFDEF FPC}
{$IF (not defined(WINDOWS)) AND (FPC_FULLVERSION>=20501)}cdecl{$ELSE}stdcall{$IFEND}
{$ELSE}stdcall{$ENDIF};
begin
  if GetInterface(IID, Obj) then
    Result := 0
  else
    Result := E_NOINTERFACE;
end;

function TTemplateInterface._AddRef: Integer;
{$IFDEF FPC}
{$IF (not defined(WINDOWS)) AND (FPC_FULLVERSION>=20501)}cdecl{$ELSE}stdcall{$IFEND}
{$ELSE}stdcall{$ENDIF};
begin
  Result := 1;
end;

function TTemplateInterface._Release: Integer;
{$IFDEF FPC}
{$IF (not defined(WINDOWS)) AND (FPC_FULLVERSION>=20501)}cdecl{$ELSE}stdcall{$IFEND}
{$ELSE}stdcall{$ENDIF};
begin
  Result := 1;
end;

end.
