Usage info:
  tomovee_cli [options] <command> [arguments]

Options:
   -c <count>        Show <count> files at maximum
   -s <storage>      Restrict queries to <storage>

Commands:
   new               Show of new files/movies
   search <pattern>  Search for <pattern> in the cached filenames
   export            Export database into json to stdout
   import            Import database into json from stdin
   diff +st1,-st2    Compares two storages
   help              Show this help

