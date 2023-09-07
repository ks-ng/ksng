bytestring = list[int]

class Tera:
	@staticmethod
	def getDistinctValues(data: bytestring) -> bytestring:
		return list(set(data))
