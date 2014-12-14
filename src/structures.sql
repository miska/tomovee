CREATE TABLE IF NOT EXISTS files  (id INTEGER PRIMARY KEY AUTOINCREMENT, 
                                   md5hash CHAR(18),
                                   osdbhash BIGINT,
                                   size BIGINT, 
                                   added TIMESTAMP, checked TIMESTAMP);
CREATE TABLE IF NOT EXISTS movies (id INTEGER PRIMARY KEY AUTOINCREMENT,
                                   imdb_id TEXT,
                                   orig_name TEXT,
                                   released TEXT);
CREATE TABLE IF NOT EXISTS paths  (id INTEGER PRIMARY KEY AUTOINCREMENT, 
                                   storage TEXT,
                                   path TEXT NOT NULL,
                                   file_id INTEGER,
                                   movie_id INTEGER,
                                   FOREIGN KEY(file_id) REFERENCES file(id),
                                   FOREIGN KEY(movie_id) REFERENCES movies(id));
