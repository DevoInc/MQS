function mariadb_query_execute(query)
   if query:sub(1, 6) == "INSERT" then
      print(query)
    end
end
