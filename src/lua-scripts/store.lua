local utils = require("mqs-utils")

dbdir = "/var/lib/mqs/"
if not utils.is_dir(dbdir) then
   utils.mkdir(dbdir, 504)
end

journal_file = "/indicators.csv"

print("Starting...")
   
function mariadb_query_execute(query)
   if query:sub(1, 31) == "INSERT INTO `misp`.`attributes`" then
      values = query:match("VALUES %((.*)%)")
      csv = utils.parse_csv_line(values, ',')

      indicator_day = os.date("%Y-%m-%d", csv[3])      
      indicator_folder = dbdir .. indicator_day
      -- print(indicator_folder)
      
      if not utils.is_dir(indicator_folder) then
      	 utils.mkdir(indicator_folder, 504)
	 local fp = io.open(indicator_folder .. journal_file, "w")
	 fp:write("object_id, to_ids, timestamp, distribution, deleted, disable_correlation, category, type, event_id, sharing_group_id, comment, uuid, value1, value2\n")
	 fp:close()
      else
	 -- FIXME: We open our file for each indicator. This is dumb.
	 -- I am doing this because I don't want to add a lot of logic to maintain a
	 -- reasonable amount of simultaneous opened files at the same time
	 -- AND we don't have zillions of indicators.
	 -- However this can be greatly improved, with things like keeping every day of the actual
	 -- months files opened etc.
	 -- Reading from the network can have a rather large time span, so I simplify here but it is not great performance wise
	 fp = io.open(indicator_folder .. journal_file, "a")
	 fp:write(csv[1] .. "," ..
		  csv[2] .. "," ..
		  csv[3] .. "," ..
		  csv[4] .. "," ..
		  csv[5] .. "," ..
		  csv[6] .. "," ..
		  csv[7] .. "," ..
		  csv[8] .. "," ..
		  csv[9] .. "," ..
		  csv[10] .. "," ..
		  csv[11] .. "," ..
		  csv[12] .. "," ..
		  csv[13] .. "," ..
		  csv[14] .. "\n")
	 io.close(fp)
      end
      
   end
end
