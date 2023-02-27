PULSONIX_LIBRARY_ASCII "SamacSys ECAD Model"
//464342/219406/2.49/11/4/Connector

(asciiHeader
	(fileUnits MM)
)
(library Library_1
	(padStyleDef "r180_70"
		(holeDiam 0)
		(padShape (layerNumRef 1) (padShapeType Rect)  (shapeWidth 0.700) (shapeHeight 1.800))
		(padShape (layerNumRef 16) (padShapeType Ellipse)  (shapeWidth 0) (shapeHeight 0))
	)
	(padStyleDef "c380_h269.26"
		(holeDiam 2.6926)
		(padShape (layerNumRef 1) (padShapeType Ellipse)  (shapeWidth 3.800) (shapeHeight 3.800))
		(padShape (layerNumRef 16) (padShapeType Ellipse)  (shapeWidth 3.800) (shapeHeight 3.800))
	)
	(padStyleDef "c55_h110"
		(holeDiam 1.1)
		(padShape (layerNumRef 1) (padShapeType Ellipse)  (shapeWidth 0.550) (shapeHeight 0.550))
		(padShape (layerNumRef 16) (padShapeType Ellipse)  (shapeWidth 0.550) (shapeHeight 0.550))
	)
	(textStyleDef "Normal"
		(font
			(fontType Stroke)
			(fontFace "Helvetica")
			(fontHeight 1.27)
			(strokeWidth 0.127)
		)
	)
	(patternDef "692112030100" (originalName "692112030100")
		(multiLayer
			(pad (padNum 1) (padStyleRef r180_70) (pt 4.000, 2.275) (rotation 0))
			(pad (padNum 2) (padStyleRef r180_70) (pt 3.000, 2.275) (rotation 0))
			(pad (padNum 3) (padStyleRef r180_70) (pt 2.000, 2.275) (rotation 0))
			(pad (padNum 4) (padStyleRef r180_70) (pt 1.000, 2.275) (rotation 0))
			(pad (padNum 5) (padStyleRef r180_70) (pt 0.000, 2.275) (rotation 0))
			(pad (padNum 6) (padStyleRef r180_70) (pt -1.000, 2.275) (rotation 0))
			(pad (padNum 7) (padStyleRef r180_70) (pt -2.000, 2.275) (rotation 0))
			(pad (padNum 8) (padStyleRef r180_70) (pt -3.000, 2.275) (rotation 0))
			(pad (padNum 9) (padStyleRef r180_70) (pt -4.000, 2.275) (rotation 0))
			(pad (padNum 10) (padStyleRef c380_h269.26) (pt -5.850, -0.175) (rotation 90))
			(pad (padNum 11) (padStyleRef c380_h269.26) (pt 5.850, -0.175) (rotation 90))
			(pad (padNum 12) (padStyleRef c55_h110) (pt -2.250, -0.175) (rotation 90))
			(pad (padNum 13) (padStyleRef c55_h110) (pt 2.250, -0.175) (rotation 90))
		)
		(layerContents (layerNumRef 18)
			(attr "RefDes" "RefDes" (pt 0.000, 0.000) (textStyleRef "Normal") (isVisible True))
		)
		(layerContents (layerNumRef 28)
			(line (pt -6 0.735) (pt 6 0.735) (width 0.025))
		)
		(layerContents (layerNumRef 28)
			(line (pt 6 0.735) (pt 6 -3.175) (width 0.025))
		)
		(layerContents (layerNumRef 28)
			(line (pt 6 -3.175) (pt -6 -3.175) (width 0.025))
		)
		(layerContents (layerNumRef 28)
			(line (pt -6 -3.175) (pt -6 0.735) (width 0.025))
		)
		(layerContents (layerNumRef Courtyard_Top)
			(line (pt -8.75 4.175) (pt 8.75 4.175) (width 0.1))
		)
		(layerContents (layerNumRef Courtyard_Top)
			(line (pt 8.75 4.175) (pt 8.75 -4.175) (width 0.1))
		)
		(layerContents (layerNumRef Courtyard_Top)
			(line (pt 8.75 -4.175) (pt -8.75 -4.175) (width 0.1))
		)
		(layerContents (layerNumRef Courtyard_Top)
			(line (pt -8.75 -4.175) (pt -8.75 4.175) (width 0.1))
		)
		(layerContents (layerNumRef 18)
			(line (pt -6 -2.715) (pt -6 -3.175) (width 0.1))
		)
		(layerContents (layerNumRef 18)
			(line (pt -6 -3.175) (pt 6 -3.175) (width 0.1))
		)
		(layerContents (layerNumRef 18)
			(line (pt 6 -3.175) (pt 6 -2.715) (width 0.1))
		)
		(layerContents (layerNumRef 18)
			(line (pt -3 0.735) (pt 3 0.735) (width 0.1))
		)
	)
	(symbolDef "692112030100" (originalName "692112030100")

		(pin (pinNum 1) (pt 900 mils -800 mils) (rotation 180) (pinLength 200 mils) (pinDisplay (dispPinName true)) (pinName (text (pt 670 mils -825 mils) (rotation 0]) (justify "Right") (textStyleRef "Normal"))
		))
		(pin (pinNum 2) (pt 900 mils -700 mils) (rotation 180) (pinLength 200 mils) (pinDisplay (dispPinName true)) (pinName (text (pt 670 mils -725 mils) (rotation 0]) (justify "Right") (textStyleRef "Normal"))
		))
		(pin (pinNum 3) (pt 900 mils -600 mils) (rotation 180) (pinLength 200 mils) (pinDisplay (dispPinName true)) (pinName (text (pt 670 mils -625 mils) (rotation 0]) (justify "Right") (textStyleRef "Normal"))
		))
		(pin (pinNum 4) (pt 900 mils -500 mils) (rotation 180) (pinLength 200 mils) (pinDisplay (dispPinName true)) (pinName (text (pt 670 mils -525 mils) (rotation 0]) (justify "Right") (textStyleRef "Normal"))
		))
		(pin (pinNum 5) (pt 900 mils -400 mils) (rotation 180) (pinLength 200 mils) (pinDisplay (dispPinName true)) (pinName (text (pt 670 mils -425 mils) (rotation 0]) (justify "Right") (textStyleRef "Normal"))
		))
		(pin (pinNum 6) (pt 900 mils -300 mils) (rotation 180) (pinLength 200 mils) (pinDisplay (dispPinName true)) (pinName (text (pt 670 mils -325 mils) (rotation 0]) (justify "Right") (textStyleRef "Normal"))
		))
		(pin (pinNum 7) (pt 900 mils -200 mils) (rotation 180) (pinLength 200 mils) (pinDisplay (dispPinName true)) (pinName (text (pt 670 mils -225 mils) (rotation 0]) (justify "Right") (textStyleRef "Normal"))
		))
		(pin (pinNum 8) (pt 900 mils -100 mils) (rotation 180) (pinLength 200 mils) (pinDisplay (dispPinName true)) (pinName (text (pt 670 mils -125 mils) (rotation 0]) (justify "Right") (textStyleRef "Normal"))
		))
		(pin (pinNum 9) (pt 900 mils 0 mils) (rotation 180) (pinLength 200 mils) (pinDisplay (dispPinName true)) (pinName (text (pt 670 mils -25 mils) (rotation 0]) (justify "Right") (textStyleRef "Normal"))
		))
		(pin (pinNum 10) (pt 0 mils 0 mils) (rotation 0) (pinLength 200 mils) (pinDisplay (dispPinName true)) (pinName (text (pt 230 mils -25 mils) (rotation 0]) (justify "Left") (textStyleRef "Normal"))
		))
		(pin (pinNum 11) (pt 0 mils -100 mils) (rotation 0) (pinLength 200 mils) (pinDisplay (dispPinName true)) (pinName (text (pt 230 mils -125 mils) (rotation 0]) (justify "Left") (textStyleRef "Normal"))
		))
		(line (pt 200 mils 100 mils) (pt 700 mils 100 mils) (width 6 mils))
		(line (pt 700 mils 100 mils) (pt 700 mils -900 mils) (width 6 mils))
		(line (pt 700 mils -900 mils) (pt 200 mils -900 mils) (width 6 mils))
		(line (pt 200 mils -900 mils) (pt 200 mils 100 mils) (width 6 mils))
		(attr "RefDes" "RefDes" (pt 750 mils 300 mils) (justify Left) (isVisible True) (textStyleRef "Normal"))
		(attr "Type" "Type" (pt 750 mils 200 mils) (justify Left) (isVisible True) (textStyleRef "Normal"))

	)
	(compDef "692112030100" (originalName "692112030100") (compHeader (numPins 11) (numParts 1) (refDesPrefix J)
		)
		(compPin "1" (pinName "1") (partNum 1) (symPinNum 1) (gateEq 0) (pinEq 0) (pinType Unknown))
		(compPin "2" (pinName "2") (partNum 1) (symPinNum 2) (gateEq 0) (pinEq 0) (pinType Unknown))
		(compPin "3" (pinName "3") (partNum 1) (symPinNum 3) (gateEq 0) (pinEq 0) (pinType Unknown))
		(compPin "4" (pinName "4") (partNum 1) (symPinNum 4) (gateEq 0) (pinEq 0) (pinType Unknown))
		(compPin "5" (pinName "5") (partNum 1) (symPinNum 5) (gateEq 0) (pinEq 0) (pinType Unknown))
		(compPin "6" (pinName "6") (partNum 1) (symPinNum 6) (gateEq 0) (pinEq 0) (pinType Unknown))
		(compPin "7" (pinName "7") (partNum 1) (symPinNum 7) (gateEq 0) (pinEq 0) (pinType Unknown))
		(compPin "8" (pinName "8") (partNum 1) (symPinNum 8) (gateEq 0) (pinEq 0) (pinType Unknown))
		(compPin "9" (pinName "9") (partNum 1) (symPinNum 9) (gateEq 0) (pinEq 0) (pinType Unknown))
		(compPin "10" (pinName "10") (partNum 1) (symPinNum 10) (gateEq 0) (pinEq 0) (pinType Unknown))
		(compPin "11" (pinName "11") (partNum 1) (symPinNum 11) (gateEq 0) (pinEq 0) (pinType Unknown))
		(attachedSymbol (partNum 1) (altType Normal) (symbolName "692112030100"))
		(attachedPattern (patternNum 1) (patternName "692112030100")
			(numPads 11)
			(padPinMap
				(padNum 1) (compPinRef "1")
				(padNum 2) (compPinRef "2")
				(padNum 3) (compPinRef "3")
				(padNum 4) (compPinRef "4")
				(padNum 5) (compPinRef "5")
				(padNum 6) (compPinRef "6")
				(padNum 7) (compPinRef "7")
				(padNum 8) (compPinRef "8")
				(padNum 9) (compPinRef "9")
				(padNum 10) (compPinRef "10")
				(padNum 11) (compPinRef "11")
			)
		)
		(attr "Mouser2 Part Number" "710-692112030100")
		(attr "Mouser2 Price/Stock" "https://www.mouser.com/Search/Refine.aspx?Keyword=710-692112030100")
		(attr "Manufacturer_Name" "Wurth Elektronik")
		(attr "Manufacturer_Part_Number" "692112030100")
		(attr "Description" "Wurth Elektronik Male Right Angle Through Hole Version 3 Type A USB Connector, 30 V ac, 0.25A WR-COM")
		(attr "<Hyperlink>" "https://componentsearchengine.com//692112030100.pdf")
		(attr "<Component Height>" "1")
		(attr "<STEP Filename>" "692112030100.stp")
		(attr "<STEP Offsets>" "X=0;Y=-10.74;Z=-1.3")
		(attr "<STEP Rotation>" "X=0;Y=0;Z=0")
	)

)
