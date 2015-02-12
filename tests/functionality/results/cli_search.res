tomovee_cli search ever
File:
  added:      2014-09-26 11:00:00
  resolution: 320x240
  length:     0:01:57
  paths:
    test1://a.avi
    test1://inside/b.avi
    test1://inside/whatever.avi
    test2://c.avi
tomovee_cli -s test1 search ever
File:
  added:      2014-09-26 11:00:00
  resolution: 320x240
  length:     0:01:57
  paths:
    test1://a.avi
    test1://inside/b.avi
    test1://inside/whatever.avi
    test2://c.avi
tomovee_cli -s test2 search ever
