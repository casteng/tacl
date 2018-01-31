{
@abstract(Template collections test)

This is test console runner for template collections and algorithms

@author(George Bakhtadze (avagames@gmail.com))
}

program TemplateTest;

{$APPTYPE CONSOLE}

uses
  tester,
  CollectionTest,
  SortTest;

begin
  tester.RunTests();
  Readln;
end.
