(*
  Template linked list collection option constants
*)
  {$IF (dsRangeCheck in _LinkedListOptions)}
    {$DEFINE _RANGECHECK}
  {$ELSE}
    {$UNDEF _RANGECHECK}
  {$IFEND}
