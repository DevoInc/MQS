function mariadb_query_execute(query)
	 local fp = io.open("queries.log", "a")
	 fp:write(os.date("[%Y-%m-%d %H:%M:%S] ") .. query .. "\n")
	 fp:close()
end
