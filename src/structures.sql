CREATE TABLE IF NOT EXISTS files  (id INTEGER PRIMARY KEY AUTOINCREMENT, 
                                   osdbhash BIGINT,
                                   mhash INT,
                                   size BIGINT, 
                                   added TIMESTAMP,
                                   movie_id INTEGER,
                                   FOREIGN KEY(movie_id) REFERENCES movies(id));
CREATE TABLE IF NOT EXISTS movies (id INTEGER PRIMARY KEY AUTOINCREMENT,
                                   imdb_id TEXT,
                                   orig_name TEXT,
                                   released TEXT);
CREATE TABLE IF NOT EXISTS paths  (id INTEGER PRIMARY KEY AUTOINCREMENT, 
                                   storage TEXT,
                                   path TEXT NOT NULL,
                                   file_id INTEGER,
                                   checked TIMESTAMP,
                                   FOREIGN KEY(file_id) REFERENCES file(id));
