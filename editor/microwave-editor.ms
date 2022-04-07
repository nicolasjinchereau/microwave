-- Max Settings:
--    System Units: Meters
--    Display Unit Scale: Metric/Meters
--    Set diffuse bitmap's 'Alpha Source' to 'None' for Blend One/Zero/Opaque or 'Image Alpha' for SrcAlpha/OneMinusSrcAlpha/Transparent,
--    Collision:
--        Add MassFX_RigidBody modifier.
--        Set 'Shape Type' to 'Sphere', 'Box', 'Capsule' or 'Convex'.
--        Set 'Rigid Body Type' to Static, Dynamic, or Kinematic.
--        Bake object with this script to generate collider pivot and User Defined Properties for importer.
--    Set object 'Visibility' to '0' for importer to delete mesh (for invisible colliders). Use 'Display as Box' to retain visibility in 3DSMax.
--    3DS Max 'Hide'/eyeball setting exposed as Node::active in engine
--
-- FBX Export:
--    Units: Check option 'Automatic' ('Scale Factor' should be 1.0 for meters)
--    Up Axis: Set to 'Z-up'
--    Importer generates materials and searches for texture asset by filename
--    Supported:
--        Meshe objects (with Skin or MassFX_RigidBody modifiers)
--        Dummy objects (converted to empty nodes)
--        Materials (opacity, diffuse texture/color, specular texture/color/shininess, bump map)
--        Node position/rotation/scale animation

rollout MicrowaveEditor "Microwave Editor" width:440 height:300
(
	group "Rigid Body Selection"
	(
		button btn_SelectBodiesDynamic "Select Dynamic" width:100 height:30 across:4
		button btn_SelectBodiesKinematic "Select Kinematic" width:100 height:30
		button btn_SelectBodiesStatic "Select Static" width:100 height:30
		button btn_SelectBodiesAll "Select All" width:100 height:30
	)
	
	group "Collision Baking"
	(
		button btn_BakeCollidersSel "Bake Selected" width:100 height:30 across:4
		button btn_ClearCollidersSel "Clear Selected" width:100 height:30
		button btn_BakeCollidersAll "Bake All" width:100 height:30
		button btn_ClearCollidersAll "Clear All" width:100 height:30
		progressBar prg_GenerateColliders value:0 color:(color 0 160 0) orient:#horizontal
	)
	
	fn GetMeshTypeName meshType =
	(
		if meshType == 1 then return "Sphere"
		else if meshType == 2 then return "Box"
		else if meshType == 3 then return "Capsule"
		else if meshType == 4 then return "Convex"
		else if meshType == 5 then return "Original"
		else if meshType == 6 then return "Custom"
		else if meshType == 7 then return "Concave"
		else return "Invalid"
	)
	
	fn IsMeshTypeSupported meshType =
	(
		if meshType == 1 then return true --"Sphere"
		else if meshType == 2 then return true --"Box"
		else if meshType == 3 then return true --"Capsule"
		else if meshType == 4 then return true --"Convex"
		else if meshType == 5 then return false --"Original"
		else if meshType == 6 then return false --"Custom"
		else if meshType == 7 then return false --"Concave"
		else return false
	)
	
	fn GetBodyTypeName bodyType =
	(
		if bodyType == 1 then return "Dynamic"
		else if bodyType == 2 then return "Kinematic"
		else if bodyType == 3 then return "Static"
		else return "Invalid"
	)
	
	fn CreateColliderNode =
	(
		local pt = Point name:"point"
		pt.size = 0.4
		pt.centermarker = false
		pt.axistripod = false
		pt.cross = false
		pt.box = true
		pt.drawOnTop = true
		pt.constantScreenSize = false
		--obj.isHidden = true
		return pt
	)
	
	fn BakeColliders bakeAll = 
	(
		--prevents access violation (opens modify panel)
		max modify mode
		
		undo "Bake Colliders" on
		(
			print "Baking Collision..."
			
			colliderMaterial = standardMaterial name:"Collider_Wireframe" diffuse:(color 0 255 0)
			colliderMaterial.wire = true
			colliderMaterial.selfIllumAmount = 100
			
			local currentObjects = #()
			
			if bakeAll then (
				currentObjects = objects as array
			) else (
				currentObjects = selection as array
			)
			
			local objectCount = currentObjects.count
			local objectsDone = 0
			
			for obj in currentObjects do
			(
				objectsDone += 1
				local percent = ((objectsDone as float) / (objectCount as float) * 100.0)
				prg_GenerateColliders.value = percent
				
				for m in obj.modifiers do
				(
					if classof m == MassFX_RBody then
					(
						local rb = m
						
						meshType = rb.meshType
						bodyType = rb.type
						
						meshTypeName = GetMeshTypeName meshType
						bodyTypeName = GetBodyTypeName bodyType
						
						if IsMeshTypeSupported meshType then
						(
							--prevents access violation (displays object in modify panel while working)
							select obj
							
							-- set to dynamic to enable all modify panel features
							rb.type = 1
							
							rb.RBMeshRegenerate 1
							rb.MassCenterMode = 2
							rb.CalculateMassCenter()
							
							setUserProp obj "ColliderType" meshTypeName
							setUserProp obj "ColliderBodyType" bodyTypeName
							setUserProp obj "ColliderFriction" rb.dynamicFriction
							
 							local colliderNodeName = genguid()
 							setUserProp obj "ColliderNode" colliderNodeName
 							
							if meshType == 1 then --Sphere
							(
								colliderNode = CreateColliderNode()
								setUserProp obj "ColliderRadius" (rb.GetRBMeshRadius 1)
							)
							else if meshType == 2 then --Box
							(
								colliderNode = CreateColliderNode()
								setUserProp obj "ColliderLength" (rb.GetRBMeshLength 1)
								setUserProp obj "ColliderWidth" (rb.GetRBMeshWidth 1)
								setUserProp obj "ColliderHeight" (rb.GetRBMeshHeight 1)
							)
							else if meshType == 3 then --Capsule
							(
								colliderNode = CreateColliderNode()
								setUserProp obj "ColliderRadius" (rb.GetRBMeshRadius 1)
								setUserProp obj "ColliderHeight" (rb.GetRBMeshHeight 1)
							)
							else if meshType == 4 then --Convex
							(
								colliderNode = rb.RBMeshConvertToCustomMesh 1
								rb.meshType = meshType -- conversion changes this, so revert it
							)
							
							setUserProp colliderNode "IsColliderNode" true
							
 							colliderNode.name = colliderNodeName
							colliderNode.parent = obj
							
							if meshType == 4 then
							(
								in coordsys world (
									colliderNode.rotation = obj.rotation
									colliderNode.position = obj.position
								)
							)
							else
							(
								in coordsys parent (
									local localRot = (rb.GetRBMeshTM 1).rotationPart
									local localPos = [rb.MassCenterX, rb.MassCenterY, rb.MassCenterZ]
									colliderNode.rotation = localRot
									colliderNode.position = localPos
								)
							)
							
							-- set Rigid Body Type back to previous value
							rb.type = bodyType
							
							print ("  Collider Generated For: " + obj.name)
						)
						else
						(
							print ("  Cannot generate collider for shape: " + meshTypeName)
						)
					)
				)
			)
			
			prg_GenerateColliders.value = 0
		)
	)
	
	fn GetAllChildren selectedNodes =
	(
		allNodes = #()
		
		for n in selectedNodes do
		(
			append allNodes n
			join allNodes (GetAllChildren n.children)
		)
		
		return allNodes
	)

	fn FindChild parentNode childName =
	(
		for n in parentNode.children do
		(
			if n.name == childName then return n
		)
		
		return undefined
	)
	
	fn ClearColliders clearAll =
	(
		undo "Clear Colliders" on
		(
			print "Clearing Colliders..."
			
			local currentObjects = #()
			
			if clearAll then (
				currentObjects = objects as array
			) else (
				currentObjects = selection as Array
			)
			
			for obj in currentObjects do
			(
				if (IsValidNode obj) == false do continue
				
				colliderType = getUserProp obj "ColliderType"
				
				if colliderType != undefined then
				(
					colliderNodeName = getUserProp obj "ColliderNode"
					colliderNode = FindChild obj colliderNodeName
					if colliderNode != undefined then delete colliderNode
					
					deleteUserProp obj "ColliderType"
					deleteUserProp obj "ColliderBodyType"
					deleteUserProp obj "ColliderFriction"
					deleteUserProp obj "ColliderNode"
					deleteUserProp obj "ColliderRadius"
					deleteUserProp obj "ColliderLength"
					deleteUserProp obj "ColliderWidth"
					deleteUserProp obj "ColliderHeight"
					deleteUserProp obj "ColliderMesh"
					
					print ("  Collider Cleared: " + obj.name)
				)
				
-- 				isColliderNode = (getUserProp obj "IsColliderNode")
-- 				if isColliderNode == true then delete obj
			)
		)
	)
	
	fn SelectBodies bodyType =
	(
		newSelection = #()
		currentObjects = objects as array
		
		for obj in currentObjects do
		(
			if obj.modifiers.count > 0 then
			(
				m = obj.modifiers[1]
				if classof m == MassFX_RBody then
				(
					local rb = m
					if bodyType == -1 or rb.type == bodyType then
					(
						append newSelection obj
					)
				)
			)
		)
		
		clearSelection()
		select newSelection
	)
	
	on btn_BakeCollidersAll pressed do (
		BakeColliders true
	)
	
	on btn_BakeCollidersSel pressed do (
		BakeColliders false
	)
	
	on btn_ClearCollidersAll pressed do (
		ClearColliders true
	)
	
	on btn_ClearCollidersSel pressed do (
		ClearColliders false
	)
	
	on btn_SelectBodiesAll pressed do (
		SelectBodies -1
	)
	
	on btn_SelectBodiesDynamic pressed do (
		SelectBodies 1
	)
	
	on btn_SelectBodiesKinematic pressed do (
		SelectBodies 2
	)
	
	on btn_SelectBodiesStatic pressed do (
		SelectBodies 3
	)
	
	on MicrowaveProperties open do (
		-- initialize window
	)
)

createDialog MicrowaveEditor style:#(#style_titlebar, #style_border, #style_sysmenu,#style_resizing,#style_maximizebox)
